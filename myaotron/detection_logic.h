#ifndef MYAOTRON_DETECTION_LOGIC_H
#define MYAOTRON_DETECTION_LOGIC_H

/**
 * Pure detection geometry — no Arduino or HUSKYLENS dependencies.
 * Shared between the main sketch and desktop unit tests.
 */

#include <stdint.h>

struct BBox {
  int16_t x, y, w, h;
  int8_t confidence;
  bool valid;
};

// Returns the horizontal overlap ratio of box A within box B.
// 1.0 = A is fully inside B horizontally; 0.0 = no overlap.
inline float horizontalOverlap(BBox a, BBox b) {
  int16_t aLeft  = a.x - a.w / 2;
  int16_t aRight = a.x + a.w / 2;
  int16_t bLeft  = b.x - b.w / 2;
  int16_t bRight = b.x + b.w / 2;

  int16_t overlapLeft  = (aLeft > bLeft) ? aLeft : bLeft;
  int16_t overlapRight = (aRight < bRight) ? aRight : bRight;
  int16_t overlapWidth = overlapRight - overlapLeft;

  if (overlapWidth <= 0 || a.w <= 0) return 0.0f;
  return (float)overlapWidth / (float)a.w;
}

// Check if the cat bbox is vertically positioned on/above the surface.
// In camera coordinates, y=0 is the top of the frame.
inline bool isVerticallyOnSurface(BBox cat, BBox surface) {
  int16_t catBottom  = cat.y + cat.h / 2;
  int16_t surfaceTop = surface.y - surface.h / 2;
  int16_t surfaceBot = surface.y + surface.h / 2;

  return (catBottom >= surfaceTop && catBottom <= surfaceBot);
}

// Check if a BBox meets the minimum confidence threshold.
inline bool meetsConfidence(BBox box, int8_t minConfidence) {
  return box.valid && box.confidence >= minConfidence;
}

// Full "cat on surface" check combining overlap and vertical position.
inline bool isCatOnSurface(BBox cat, BBox surface, float overlapThreshold) {
  if (!cat.valid || !surface.valid) return false;
  return horizontalOverlap(cat, surface) >= overlapThreshold
      && isVerticallyOnSurface(cat, surface);
}

#endif // MYAOTRON_DETECTION_LOGIC_H
