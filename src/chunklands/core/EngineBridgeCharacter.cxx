
#include "EngineBridge.hxx"
#include "resolver.hxx"

namespace chunklands::core {

void EngineBridge::JSCall_characterSetCollision(JSCbi info)
{
    resolve(info.Env(), engine_->CharacterSetCollision(info[0].ToBoolean()));
}

JSValue EngineBridge::JSCall_characterIsCollision(JSCbi info)
{
    return resolve(info.Env(), engine_->CharacterIsCollision(), [&](bool collision) {
        return JSBoolean::New(info.Env(), collision);
    });
}

} // namespace chunklands::core