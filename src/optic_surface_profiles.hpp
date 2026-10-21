#pragma once
namespace ray_optics {
struct Profile{U32 checksum,bone;Vec3 center;float radius;U32 semantic;float zoomFov[2];const char*material;};
static const Profile profiles[]={
{1095087377u,3u,{0.003201975f,6.403480644f,-1.285474145f},0.421300963f,3u,{45.000000000f,45.000000000f},"models/weapons/v_models/rif_aug/rif_aug_scope_glass"},
{1273104850u,3u,{-0.031827704f,5.807426597f,-0.153885347f},0.715839863f,4u,{40.000000000f,10.000000000f},"models/weapons/shared/scope/scope_awp"},
};
inline const Profile*profile(U32 checksum){for(const auto&p:profiles)if(p.checksum==checksum)return &p;return nullptr;}
}
