#ifndef CARCASSONNE_ASSETS_ASSETS_H_
#define CARCASSONNE_ASSETS_ASSETS_H_

#include <unordered_map>

#include "assets/texture.h"

namespace carcassonne {

namespace assets {

class Assets
{
public:
   Assets();

   std::shared_ptr<Texture> getTexture(const std::string& name);

private:

   std::unordered_map<std::weak_ptr<Texture> > textures_;


   Assets(const Assets&);
   void operator=(const Assets&);
};

} // namespace assets

} // namespace carcassonne

#endif
