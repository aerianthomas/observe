#include "svg_frame.h"

namespace observe
{
  void SvgFrame::setDimensions()
  {
    int m = margin_.compute(dpiScale(), nativeWidth(), nativeHeight(), 0.0f);
    svg_.setDimensions(width() - 2 * m / dpiScale(), height() - 2 * m / dpiScale(), dpiScale());

    if (sub_frame_ == nullptr && svg_.width() && svg_.height())
    {
      sub_frame_ = std::make_unique<SubFrame>(svg_.drawable(), &context_);
      addChild(sub_frame_.get());
    }

    if (sub_frame_)
    {
      sub_frame_->setNativeBounds(m + svg_.drawable()->post_bounding_box.x() * dpiScale(),
                                  m + svg_.drawable()->post_bounding_box.y() * dpiScale(),
                                  nativeWidth() - 2 * m, nativeHeight() - 2 * m);
    }
  }

  void SvgFrame::loadSubFrames()
  {
    sub_frame_ = nullptr;
    setDimensions();
  }
}