/*
 * \processor_tracker_feature.h
 *
 *  Created on: 27/02/2016
 *      \author: jsola
 */

#ifndef PROCESSOR_TRACKER_FEATURE_H_
#define PROCESSOR_TRACKER_FEATURE_H_

#include "processor_tracker.h"
#include "capture_base.h"

/** \brief General tracker processor
 *
 * This class implements the incremental tracker. It contains three pointers to three Captures of type CaptureBase, named \b origin, \b last and \b incoming:
 *   - \b origin: this points to a Capture where all Feature tracks start.
 *   - \b last: the last Capture tracked by the tracker. A sufficient subset of the Features in \b origin is still alive in \b last.
 *   - \b incoming: the capture being received. The tracker operates on this Capture,
 *     establishing correspondences between the features here and the features in \b origin. Each successful correspondence
 *     results in an extension of the track of the Feature up to the \b incoming Capture.
 *
 * A tracker can be designed to track either Features or Landmarks. This property must be set at construction time.
 *   - If tracking Features, it establishes constraints Feature-Feature;
 *     it does not use Landmarks, nor it creates Landmarks.
 *   - If tracking Landmarks, it establishes constraints Feature-Landmark;
 *     it uses Landmarks for tracking, in an active-search approach,
 *     and it creates Landmarks with each new Feature detected.
 *
 * The pipeline of actions for an autonomous tracker can be resumed as follows:
 *   - Init the tracker with an \b origin Capture: init();
 *   - On each incoming Capture,
 *     - Track known features in the \b incoming Capture: processKnownFeatures();
 *     - Check if enough Features are still tracked, and vote for a new KeyFrame if this number is too low:
 *     - if voteForKeyFrame()
 *       - Look for new Features and make Landmarks with them:
 *       - detectNewFeatures()
 *       - if we use landmarks, do for each detected Feature:
 *         - create landmarks: createOneLandmark()
 *       - create constraints Feature-Landmark or Feature-Feature: createConstraint()
 *       - Make a KeyFrame with the \b last Capture: makeKeyFrame();
 *       - Reset the tracker with the \b last Capture as the new \b origin: reset();
 *     - else
 *       - Advance the tracker one Capture ahead: advance()
 *
 * This functionality exists by default in the virtual method process(). You can overload it at your convenience.
 *
 * This is an abstract class. The following pure virtual methods have to be implemented in derived classes:
 *   - processKnownFeatures()
 *   - voteForKeyFrame()
 *   - detectNewFeatures()
 *   - createLandmark()
 *   - createConstraint()
 */
class ProcessorTrackerFeature : public ProcessorTracker
{
    public:

        /** \brief Constructor with options
         * \param _autonomous Set to make the tracker autonomous to create KeyFrames and/or Landmarks.
         * \param _uses_landmarks Set to make the tracker work with Landmarks. Un-set to work only with Features.
         */
        ProcessorTrackerFeature(ProcessorType _tp);
        virtual ~ProcessorTrackerFeature();

    protected:

        /** \brief Detect new Features
         * \param _capture_ptr Capture for feature detection. Defaults to incoming_ptr_.
         * \param _new_features_list The list of detected Features. Defaults to member new_features_list_.
         * \return The number of detected Features.
         *
         * This function detects Features that do not correspond to known Features/Landmarks in the system.
         *
         * The function sets the member new_features_list_, the list of newly detected features,
         * to be used for landmark initialization.
         */
        virtual unsigned int detectNewFeatures() = 0;

        /** \brief Track provided features from \b last to \b incoming
         * \param _feature_list_in input list of features in \b last to track
         * \param _feature_list_out returned list of features found in \b incoming
         */
        virtual unsigned int track(const FeatureBaseList& _feature_list_in, FeatureBaseList& _feature_list_out) = 0;

//        /** \brief Create one landmark
//         *
//         * Implement in derived classes to build the type of landmark you need for this tracker.
//         */
//        virtual LandmarkBase* createLandmark(FeatureBase* _feature_ptr) = 0;

        /** \brief Create a new constraint
         * \param _feature_ptr pointer to the Feature to constrain
         * \param _node_ptr NodeBase pointer to the other entity constrained. It can only be of the types FeatureBase and LandmarkBase.
         *
         * This function will be called with one of these options (and hence the second parameter NodeBase *):
         *  - createConstraint(FeatureBase *, FeatureBase *)
         *  - createConstraint(FeatureBase *, LandmarkBase *)
         *
         * Implement this method in derived classes to build the type of constraint
         * appropriate for the pair feature-feature or feature-landmark used by this tracker.
         *
         * TODO: Make a general ConstraintFactory, and put it in WolfProblem.
         * This factory only needs to know the two derived pointers to decide on the actual Constraint created.
         */
        virtual ConstraintBase* createConstraint(FeatureBase* _feature_ptr, FeatureBase* _node_ptr) = 0;

    protected:

        /**\brief Process new Features
         *
         */
        virtual unsigned int processNew();
};

#endif /* PROCESSOR_TRACKER_FEATURE_H_ */