#pragma once
namespace grenade_control {
using namespace nova;
// Exact original client RTTI, authenticated by the pinned launcher/client identity.
static const U32 grenadeVtables[]={0xc9edc4u,0xc9e3dcu,0xca5e90u,0xc9c758u,0xca2620u,0xca1d18u};
// Copied into one verified command; no render-side object is borrowed by the
// server and the emission hook never substitutes a newer tracking position.
struct Input {
 U32 player{},weapon{},ownerHandle{},weaponHandle{},kind{};
 U64 epoch{},sample{},poseMs{};Vec3 gripDelta{};
};
}
