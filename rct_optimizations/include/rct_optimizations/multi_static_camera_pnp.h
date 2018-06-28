/*
 * This file mirrors the interface for ExtrinsicStaticCameraMovingTarget found in
 * rct_optimizations/extrinsic_static_camera.h but extends it to work for simultaneously
 * localizing multiple cameras.
 *
 * Thus you now pass:
 *  1. A vector of intrinsics, one for each camera
 *  2. A vector of vector of wrist poses, one for each camera
 *  2. A vector of vector of image observations, one for each camera
 *  3. A vector of base to camera guesses, one for each camera
 *
 * author: Levi Armstrong
 */

#ifndef RCT_MULTI_STATIC_CAMERA_PNP_H
#define RCT_MULTI_STATIC_CAMERA_PNP_H

#include "rct_optimizations/types.h"
#include <Eigen/Dense>
#include <vector>

namespace rct_optimizations
{

struct MultiStaticCameraPnPProblem
{
  /** @brief The basic camera intrinsic propeties: fx, fy, cx, cy used to reproject points;
      one for each camera */
  std::vector<CameraIntrinsics> intr;

  /** @brief Your best guess for transforms, "base to target", for a given observation set taken.*/
  Eigen::Affine3d base_to_target_guess;

  /** @brief A sequence of observation sets corresponding to the image locations.
   * Each observation set consists of a set of correspodences: a 3D position (e.g. a dot) in "target
   * frame" to the image location it was detected at (2D). The outer-most vector is for each camera,
   * the inner vector is the images valid for that camera.
   */
  std::vector<CorrespondenceSet> image_observations;

  /** @brief The "base frame" to "camera frame" transform; one for each camera */
  std::vector<Eigen::Affine3d> base_to_camera;
};

struct MultiStaticCameraPnPResult
{
  /**
   * @brief Whether the underlying solver converged. If this is false, your calibration did not go well.
   * If this is true, your calibration MAY have gone well.
   */
  bool converged;

  /**
   * @brief The initial reprojection error (in pixels) per residual based on your input guesses.
   */
  double initial_cost_per_obs;

  /**
   * @brief The final reprojection error (in pixels) per residual after optimization. Note that each circle
   * has two residuals: a U and V error in the image. So a value of 1.2 means that each circle was described
   * to within 1.2 pixels in X and 1.2 pixels in Y.
   *
   * A low value here is encouraging if you had a diversity of images. If you took few images, you can get
   * a low score without getting a calibration that describes your workcell.
   */
  double final_cost_per_obs;

  /** @brief The final location of the target. */
  Eigen::Affine3d base_to_target;
};

MultiStaticCameraPnPResult optimize(const MultiStaticCameraPnPProblem& params);

}

#endif // RCT_MULTI_STATIC_CAMERA_PNP_H