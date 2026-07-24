#include "windowless_context.h"

#include <Carbon/Carbon.h>
#include <MetalKit/MetalKit.h>

namespace observe {
  class WindowlessMetalLayer {
  public:
    static CAMetalLayer* layer() { return instance().metal_layer_; }

  private:
    static WindowlessMetalLayer& instance() {
      static WindowlessMetalLayer instance;
      return instance;
    }

    WindowlessMetalLayer() {
      metal_layer_ = [CAMetalLayer layer];
      metal_layer_.colorspace = CGColorSpaceCreateWithName(kCGColorSpaceDisplayP3);
    }

    CAMetalLayer* metal_layer_ = nullptr;
  };

  void* windowlessContext() {
    return (__bridge void*)WindowlessMetalLayer::layer();
  }
}
