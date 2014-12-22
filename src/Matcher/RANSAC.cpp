/** @file RANSAC.c
 *
 * \brief The robust estimation method to find transformation based on two sets of 3D features
 * \author Michal Nowicki
 *
 */
#include "../include/Matcher/RANSAC.h"

RANSAC::RANSAC() {
	srand(time(0));
	RANSACParams.iterationCount = computeRANSACIteration(0.20);
	RANSACParams.usedPairs = 3;
	RANSACParams.inlierThreshold = 0.02;
}

// TODO: MISSING:
// - model feasibility
// - test of minimal inlierRatio of bestModel
Eigen::Matrix4f RANSAC::estimateTransformation(std::vector<Eigen::Vector3f> prevFeatures,
		std::vector<Eigen::Vector3f> features,
		std::vector<cv::DMatch> matches) {

	Eigen::Matrix4f bestTransformationModel = Eigen::Matrix4f::Identity();
	float bestInlierRatio = 0.0;

	for (int i = 0; i < RANSACParams.iterationCount; i++) {
		// Randomly select matches
		std::cout << "RANSAC: randomly sampling ids of matches" << std::endl;
		std::vector<cv::DMatch> randomMatches = getRandomMatches(matches);

		std::cout << "Matches ids : " << randomMatches[0].queryIdx << " "
				<< randomMatches[1].queryIdx << " "
				<< randomMatches[2].queryIdx << std::endl;

		std::cout << "Matches ids 2: " << randomMatches[0].trainIdx << " "
						<< randomMatches[1].trainIdx << " "
						<< randomMatches[2].trainIdx << std::endl;

		// Compute model based on those matches
		std::cout << "RANSAC: computing model based on matches" << std::endl;
		Eigen::Matrix4f transformationModel;
		bool modelComputation = computeTransformationModel(prevFeatures,
				features, randomMatches, transformationModel);

		std::cout<<std::endl<<transformationModel<<std::endl<<std::endl;

		// Check if the model is feasible ?

		// Evaluate the model
		std::cout<<"RANSAC: evaluating the model" << std::endl;
		float inlierRatio = computeInlierRatio(prevFeatures, features, matches,
				transformationModel);

		// Save better model
		std::cout<<"RANSAC: saving best model" << std::endl;
		saveBetterModel(inlierRatio, transformationModel, bestInlierRatio,
				bestTransformationModel);

		std::cout<<"RANSAC: best model inlier ratio : " << bestInlierRatio * 100.0 << "%"<< std::endl;

	}

	// Test for minimal inlierRatio of bestModel


	return bestTransformationModel;
}

// TODO: MISSING:
// - checking against deadlock
// - check if chosen points are not too close to each other
//
std::vector<cv::DMatch> RANSAC::getRandomMatches(
		const std::vector<cv::DMatch> matches) {
	const int matchesSize = matches.size();

	std::vector<cv::DMatch> chosenMatches;
	std::vector<bool> validIndex(matchesSize, true);

	// Loop until we found enough matches
	while (chosenMatches.size() < RANSACParams.usedPairs) {

		// Randomly sample one match
		int sampledMatchIndex = rand() % matchesSize;

		// Check if the match was not already chosen or is not marked as wrong
		if (validIndex[sampledMatchIndex] == true) {

			// Add sampled match
			chosenMatches.push_back(matches[sampledMatchIndex]);

			// Prevent choosing it again
			validIndex[sampledMatchIndex] = false;
		}
	}

	return chosenMatches;
}

// TODO:
// - how to handle Grisetti version?
bool RANSAC::computeTransformationModel(
		const std::vector<Eigen::Vector3f> prevFeatures,
		const std::vector<Eigen::Vector3f> features,
		const std::vector<cv::DMatch> matches,
		Eigen::Matrix4f &transformationModel) {
	Eigen::MatrixXf prevFeaturesMatrix(RANSACParams.usedPairs, 3),
			featuresMatrix(RANSACParams.usedPairs, 3);

	// Create matrices
	for (int j = 0; j < RANSACParams.usedPairs; j++) {
		cv::DMatch p = matches[j];
		prevFeaturesMatrix.block<1, 3>(j, 0) = prevFeatures[p.queryIdx];
		featuresMatrix.block<1, 3>(j, 0) = features[p.trainIdx];
	}

	// Compute transformation
	transformationModel = Eigen::umeyama(prevFeaturesMatrix.transpose(),
			featuresMatrix.transpose(), false);

	// Check if it failed
	if (std::isnan(transformationModel(0, 0))) {
		transformationModel = Eigen::Matrix4f::Identity();
		return false;
	}
	return true;
}

float RANSAC::computeInlierRatio(
		const std::vector<Eigen::Vector3f> prevFeatures,
		const std::vector<Eigen::Vector3f> features,
		const std::vector<cv::DMatch> matches,
		const Eigen::Matrix4f transformationModel) {
	// Break into rotation (R) and translation (t)
	Eigen::Matrix3f R = transformationModel.block<3, 3>(0, 0);
	Eigen::Vector3f t = transformationModel.block<3, 1>(0, 3);

	int inlierCount = 0;
	// For all matches
	for (std::vector<cv::DMatch>::const_iterator it = matches.begin();
			it != matches.end(); ++it) {
		// Estimate location of feature from position one after transformation
		Eigen::Vector3f estimatedNewPosition = R * prevFeatures[it->queryIdx]
				+ t;

		// Compute residual error and compare it to inlier threshold
		if ((estimatedNewPosition - features[it->trainIdx]).norm()
				< RANSACParams.inlierThreshold) {
			inlierCount++;
		}
	}

	// Percent of correct matches
	return float(inlierCount) / matches.size();
}

inline void RANSAC::saveBetterModel(const float inlierRatio,
		const Eigen::Matrix4f transformationModel, float &bestInlierRatio,
		Eigen::Matrix4f & bestTransformationModel) {
	if (inlierRatio > bestInlierRatio) {
		// Save better model
		bestTransformationModel = transformationModel;
		bestInlierRatio = inlierRatio;

		// Update iteration count
		RANSACParams.iterationCount = computeRANSACIteration(bestInlierRatio);
	}
}

inline int RANSAC::computeRANSACIteration(double inlierRatio, double successProbability, int numberOfPairs)
{
	return (log(1 - successProbability) / log(1 - pow(inlierRatio, numberOfPairs)));
}

