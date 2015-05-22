#include "../include/USAC/PUTSLAMEstimator.h"

//// COPIED FROM PUTSLAM:

//// TODO: MISSING:
//// - checking against deadlock
//// - check if chosen points are not too close to each other
////
//std::vector<cv::DMatch> RANSAC::getRandomMatches(
//	const std::vector<cv::DMatch> matches) {
//	const int matchesSize = matches.size();
//
//	std::vector<cv::DMatch> chosenMatches;
//	std::vector<bool> validIndex(matchesSize, true);
//
//	// Loop until we found enough matches
//	while (chosenMatches.size() < RANSACParams.usedPairs) {
//
//		// Randomly sample one match
//		int sampledMatchIndex = rand() % matchesSize;
//
//		// Check if the match was not already chosen or is not marked as wrong
//		if (validIndex[sampledMatchIndex] == true) {
//
//			// Add sampled match
//			chosenMatches.push_back(matches[sampledMatchIndex]);
//
//			// Prevent choosing it again
//			validIndex[sampledMatchIndex] = false;
//		}
//	}
//
//	return chosenMatches;
//}

// based on RANSAC::getRandomMatches from PUTSLAM
// copied TODO:
// - checking against deadlock
// - check if chosen points are not too close to each other
//
std::vector<cv::DMatch> PUTSLAMEstimator::convertUSACSamplesToPUTSLAMSamples(std::vector<unsigned int> samplesFromUSAC, const std::vector<cv::DMatch> matches)
{
	std::vector<cv::DMatch> samplesForPUTSLAM;

	for (int i = 0; i < samplesFromUSAC.size(); ++i)
	{
		samplesForPUTSLAM.push_back(matches[samplesFromUSAC[i]]);
	}

	// samples generated by USAC holds indexs of elements to use
	for (auto index : samplesFromUSAC)
	{
		samplesForPUTSLAM.push_back(matches[index]);
	}

	return samplesForPUTSLAM;
}