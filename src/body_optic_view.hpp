#pragma once
// Render consumer of the explicit rigid body. No bone calculation or cache read
// supplies the scope camera; the naturally returned draw is adapted to this body.
static U32 body_optic_equipped(U32&vm){
 U32 player=0,weapon=0,sum=0;
 if(!handcore_native::memory.read(U32((uintptr_t)client)+0x52bf6d8,&player,4)||!native_game_control::equipped_optic(player,vm,weapon,sum))return 0;
 return sum;
}
static bool body_optic_image(void*self,const U8*view,const void*hud,void*ctx,const CapturedStereo&capture,float originalFarZ){
 nova_perf::Scope timing(nova_perf::OpticImage);
 using namespace lens_material;namespace body=muzzle_native::body_authority;
 imageReady=false;
 ++imageAttempts;
 auto gate=[](U32 reason){if(reason<9)++imageReasons[reason];static U32 last=~0u;if(last!=reason){last=reason;logg.msg("O6_BODY_OPTIC_GATE",reason);}return reason==0;};
 auto&driver=handcore_native::driver;
 if(!body::rendering||!body::display.valid||!driver.leased)return gate(1);
 const auto*profile=ray_optics::profile(body::renderMount.checksum);
 if(!profile||profile->bone!=body::renderMount.bodyBone||!nativeTarget||!imageSurface||!driver.checkpoint())return gate(2);
 const auto&source=body::display;
 if(source.frame.pair!=capture.pairId||source.frame.sample!=capture.sampleId||source.frame.epoch!=capture.trackingEpoch||source.predictedTime!=capture.predictedTime)return gate(3);
 muzzle::TraceResult obstruction{};
 if(!muzzle_native::trace_segment(false,driver.lease.player,source.frame.bodyEye,source.frame.barrel.p,obstruction)||!obstruction.valid||obstruction.startsolid||obstruction.allsolid||obstruction.fraction<.99999f)return gate(4);
 constexpr U32 level=1; // Physical optics never switch off with native zoom/reload.
 lensWorld=add(muzzle::position(source.body),handcore::apply(muzzle::rotation(source.body),profile->center));
 Basis barrel=source_basis(source.frame.barrel.a);
 // Geometric 50 m zero. A fixed collimated reticle, never a moving hit marker.
 Vec3 zero=add(source.frame.barrel.p,scale(barrel.forward,50*UNITS_PER_METRE));
 Vec3 axis=sub(zero,lensWorld);float n=length(axis);if(!(n>100&&n<10000))return gate(6);
 Vec3 f=scale(axis,1/n),r=handcore::cross(f,barrel.up);n=length(r);if(n<.99f)return gate(7);r=scale(r,1/n);
 boreBasis={f,r,handcore::cross(r,f)};Angles cameraAngles=basis_angles(boreBasis);
 zoom=ray_optics::magnification(profile->zoomFov[level-1]);reticle={0,0};
 eyeWorld[0]=capture.sourceEye[0];eyeWorld[1]=capture.sourceEye[1];halfTan=ray_optics::envelope(lensWorld,profile->radius,boreBasis,eyeWorld,zoom);
 U8 camera[0x14c];memcpy(camera,view,sizeof(camera));I32 viewport[8]={0,0,0,0,768,768,768,768};memcpy(camera,viewport,sizeof(viewport));
 float fov=2*deg(atan2(halfTan,1)),aspect=1,nearZ=.5f;
 memcpy(camera+0xb0,&fov,4);memcpy(camera+0xe0,&aspect,4);memcpy(camera+0xd0,&nearZ,4);
 memcpy(camera+0xd4,&originalFarZ,4); // magnified view retains native distant detail
 memcpy(camera+0xb8,&lensWorld,12);memcpy(camera+0xc4,&cameraAngles,12);camera[0x148]&=U8(~0x0d);camera[0x68]=0;
 end_view();context_push(ctx,nativeTarget,768,768);D9Surface*color=nullptr;bool good=ok(gpu->getrt(&color))&&color;
 if(good){{nova_gpu_perf::Scope gpuTiming(nova_gpu_perf::Optic,capture.pairId);legacy_render(self,camera,hud,7,0);}good=ok(gpu->stretch(color,nullptr,imageSurface,nullptr));color->release();}
 context_pop(ctx);good=good&&driver.checkpoint();
 imageReady=good;imagePair=sourcePair=capture.pairId;
 static U32 count=0;if(good&&(++count<=4||count%120==0)){
  char line[1000];int size=_snprintf(line,sizeof(line),"NOVA_O6_BODY_OPTIC pair=%llu sample=%llu mount=%llu anchor=%llu checksum=%u zoomLevel=%u zoom=%.6f fov=%.6f lens=(%.6f,%.6f,%.6f) barrel=(%.6f,%.6f,%.6f) angles=(%.6f,%.6f,%.6f) zero=(%.6f,%.6f,%.6f) sourceAgePairs=0 forcedSetupBonesCalls=0 alwaysOn=1\r\n",capture.pairId,capture.sampleId,source.mountRevision,source.anchorRevision,profile->checksum,level,double(zoom),double(fov),double(lensWorld.x),double(lensWorld.y),double(lensWorld.z),double(source.frame.barrel.p.x),double(source.frame.barrel.p.y),double(source.frame.barrel.p.z),double(cameraAngles.p),double(cameraAngles.y),double(cameraAngles.r),double(zero.x),double(zero.y),double(zero.z));U32 written=0;if(size>0)WriteFile(logg.file,line,U32(size<999?size:999),&written,nullptr);
 }
 return gate(good?0:8);
}
