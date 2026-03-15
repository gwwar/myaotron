/**
 * Desktop unit tests for myaotron detection geometry.
 *
 * Compile and run:
 *   cd test && make
 *
 * No Arduino dependencies — tests the pure logic from detection_logic.h.
 */

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "../myaotron/detection_logic.h"

static int tests_run = 0;
static int tests_passed = 0;

#define ASSERT(expr, msg)                                                      \
  do {                                                                         \
    tests_run++;                                                               \
    if (expr) {                                                                \
      tests_passed++;                                                          \
    } else {                                                                   \
      printf("  FAIL: %s (line %d)\n", msg, __LINE__);                         \
    }                                                                          \
  } while (0)

#define ASSERT_FLOAT_EQ(a, b, msg)                                             \
  ASSERT(fabs((a) - (b)) < 0.01f, msg)

// ─── Helper ──────────────────────────────────────────────────────

BBox makeBox(int16_t x, int16_t y, int16_t w, int16_t h,
             int8_t conf = 50, bool valid = true) {
  return {x, y, w, h, conf, valid};
}

// ─── horizontalOverlap tests ─────────────────────────────────────

void test_full_overlap() {
  printf("horizontalOverlap: full overlap\n");
  // Cat (100,100,50,50) fully inside surface (100,100,200,100)
  BBox cat     = makeBox(100, 100, 50, 50);
  BBox surface = makeBox(100, 100, 200, 100);
  ASSERT_FLOAT_EQ(horizontalOverlap(cat, surface), 1.0f,
                  "cat fully inside surface should be 1.0");
}

void test_no_overlap() {
  printf("horizontalOverlap: no overlap\n");
  // Cat at x=50, surface at x=300 — no horizontal overlap
  BBox cat     = makeBox(50, 100, 40, 40);
  BBox surface = makeBox(300, 100, 100, 100);
  ASSERT_FLOAT_EQ(horizontalOverlap(cat, surface), 0.0f,
                  "disjoint boxes should be 0.0");
}

void test_partial_overlap() {
  printf("horizontalOverlap: partial overlap\n");
  // Cat: left=75, right=125 (x=100, w=50)
  // Surface: left=100, right=300 (x=200, w=200)
  // Overlap: 100–125 = 25px out of cat's 50px = 0.5
  BBox cat     = makeBox(100, 100, 50, 50);
  BBox surface = makeBox(200, 100, 200, 100);
  ASSERT_FLOAT_EQ(horizontalOverlap(cat, surface), 0.5f,
                  "half overlap should be 0.5");
}

void test_zero_width_cat() {
  printf("horizontalOverlap: zero-width cat\n");
  BBox cat     = makeBox(100, 100, 0, 50);
  BBox surface = makeBox(100, 100, 200, 100);
  ASSERT_FLOAT_EQ(horizontalOverlap(cat, surface), 0.0f,
                  "zero-width cat should be 0.0");
}

void test_cat_wider_than_surface() {
  printf("horizontalOverlap: cat wider than surface\n");
  // Cat: left=0, right=400 (x=200, w=400)
  // Surface: left=150, right=250 (x=200, w=100)
  // Overlap: 150–250 = 100px out of cat's 400px = 0.25
  BBox cat     = makeBox(200, 100, 400, 50);
  BBox surface = makeBox(200, 100, 100, 100);
  ASSERT_FLOAT_EQ(horizontalOverlap(cat, surface), 0.25f,
                  "cat wider than surface");
}

void test_adjacent_no_overlap() {
  printf("horizontalOverlap: adjacent boxes (touching)\n");
  // Cat: left=50, right=100. Surface: left=100, right=200
  BBox cat     = makeBox(75, 100, 50, 50);
  BBox surface = makeBox(150, 100, 100, 100);
  ASSERT_FLOAT_EQ(horizontalOverlap(cat, surface), 0.0f,
                  "adjacent touching boxes should be 0.0");
}

// ─── isVerticallyOnSurface tests ─────────────────────────────────

void test_cat_on_surface() {
  printf("isVerticallyOnSurface: cat sitting on surface\n");
  // Surface: top=250, bottom=350 (y=300, h=100)
  // Cat: bottom=300 (y=250, h=100) — bottom edge within surface
  BBox cat     = makeBox(200, 250, 80, 100);
  BBox surface = makeBox(200, 300, 300, 100);
  ASSERT(isVerticallyOnSurface(cat, surface),
         "cat bottom within surface bounds");
}

void test_cat_above_surface() {
  printf("isVerticallyOnSurface: cat floating above surface\n");
  // Surface: top=300, bottom=400. Cat: bottom=200 — above surface
  BBox cat     = makeBox(200, 150, 80, 100);
  BBox surface = makeBox(200, 350, 300, 100);
  ASSERT(!isVerticallyOnSurface(cat, surface),
         "cat above surface should be false");
}

void test_cat_below_surface() {
  printf("isVerticallyOnSurface: cat below surface\n");
  // Surface: top=100, bottom=200. Cat: bottom=350 — below surface
  BBox cat     = makeBox(200, 300, 80, 100);
  BBox surface = makeBox(200, 150, 300, 100);
  ASSERT(!isVerticallyOnSurface(cat, surface),
         "cat below surface should be false");
}

void test_cat_bottom_at_surface_top() {
  printf("isVerticallyOnSurface: cat bottom edge at surface top\n");
  // Surface: top=300, bottom=400. Cat: bottom=300 — exactly at edge
  BBox cat     = makeBox(200, 250, 80, 100);
  BBox surface = makeBox(200, 350, 300, 100);
  ASSERT(isVerticallyOnSurface(cat, surface),
         "cat bottom at surface top edge should be true");
}

// ─── meetsConfidence tests ───────────────────────────────────────

void test_confidence_above_threshold() {
  printf("meetsConfidence: above threshold\n");
  BBox box = makeBox(100, 100, 50, 50, 60, true);
  ASSERT(meetsConfidence(box, 30), "conf 60 >= 30 should pass");
}

void test_confidence_below_threshold() {
  printf("meetsConfidence: below threshold\n");
  BBox box = makeBox(100, 100, 50, 50, 20, true);
  ASSERT(!meetsConfidence(box, 30), "conf 20 < 30 should fail");
}

void test_confidence_invalid_box() {
  printf("meetsConfidence: invalid box\n");
  BBox box = makeBox(100, 100, 50, 50, 60, false);
  ASSERT(!meetsConfidence(box, 30), "invalid box should fail");
}

// ─── isCatOnSurface integration tests ────────────────────────────

void test_cat_on_surface_full() {
  printf("isCatOnSurface: valid cat on counter\n");
  // Cat centered on surface, good overlap
  BBox cat     = makeBox(200, 250, 80, 100, 50, true);
  BBox surface = makeBox(200, 300, 300, 100, 50, true);
  ASSERT(isCatOnSurface(cat, surface, 0.3f), "should detect cat on surface");
}

void test_cat_on_surface_no_overlap() {
  printf("isCatOnSurface: cat off to the side\n");
  BBox cat     = makeBox(50, 250, 80, 100, 50, true);
  BBox surface = makeBox(400, 300, 100, 100, 50, true);
  ASSERT(!isCatOnSurface(cat, surface, 0.3f), "no overlap should be false");
}

void test_cat_on_surface_invalid_cat() {
  printf("isCatOnSurface: invalid cat bbox\n");
  BBox cat     = makeBox(200, 250, 80, 100, 50, false);
  BBox surface = makeBox(200, 300, 300, 100, 50, true);
  ASSERT(!isCatOnSurface(cat, surface, 0.3f), "invalid cat should be false");
}

void test_cat_on_surface_invalid_surface() {
  printf("isCatOnSurface: invalid surface bbox\n");
  BBox cat     = makeBox(200, 250, 80, 100, 50, true);
  BBox surface = makeBox(200, 300, 300, 100, 50, false);
  ASSERT(!isCatOnSurface(cat, surface, 0.3f),
         "invalid surface should be false");
}

// ─── Edge cases ──────────────────────────────────────────────────

void test_negative_coordinates() {
  printf("edge case: negative coordinates\n");
  // Should not crash
  BBox cat     = makeBox(-50, -50, 100, 100, 50, true);
  BBox surface = makeBox(0, 0, 200, 200, 50, true);
  float overlap = horizontalOverlap(cat, surface);
  ASSERT(overlap >= 0.0f && overlap <= 1.0f,
         "negative coords should produce valid ratio");
}

void test_very_large_boxes() {
  printf("edge case: very large boxes\n");
  BBox cat     = makeBox(320, 240, 640, 480, 50, true);
  BBox surface = makeBox(320, 240, 640, 480, 50, true);
  ASSERT_FLOAT_EQ(horizontalOverlap(cat, surface), 1.0f,
                  "identical full-frame boxes should be 1.0");
}

// ─── Main ────────────────────────────────────────────────────────

int main() {
  printf("=== myaotron detection logic tests ===\n\n");

  // horizontalOverlap
  test_full_overlap();
  test_no_overlap();
  test_partial_overlap();
  test_zero_width_cat();
  test_cat_wider_than_surface();
  test_adjacent_no_overlap();

  // isVerticallyOnSurface
  test_cat_on_surface();
  test_cat_above_surface();
  test_cat_below_surface();
  test_cat_bottom_at_surface_top();

  // meetsConfidence
  test_confidence_above_threshold();
  test_confidence_below_threshold();
  test_confidence_invalid_box();

  // isCatOnSurface (integration)
  test_cat_on_surface_full();
  test_cat_on_surface_no_overlap();
  test_cat_on_surface_invalid_cat();
  test_cat_on_surface_invalid_surface();

  // Edge cases
  test_negative_coordinates();
  test_very_large_boxes();

  printf("\n=== Results: %d/%d passed ===\n", tests_passed, tests_run);

  return (tests_passed == tests_run) ? 0 : 1;
}
