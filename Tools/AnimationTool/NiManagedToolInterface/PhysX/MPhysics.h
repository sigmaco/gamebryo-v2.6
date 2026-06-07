// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2008 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#pragma once

#pragma warning(push)
#pragma warning(disable: 4275) // DLL export warning due to PhysX

using namespace System::Collections;

namespace NiManagedToolInterface
{
    public __gc class MSequence;
    public __gc class MSequenceGroup;
    public __gc class MPhysics
    {
    public:
        // The first few things we have regardless of whether or not PhysX is
        // being built in. It makes handling settings and registry entries
        // easier.
        
        // Constructor
        MPhysics();

        // Returns true if the Physics SDK has been successfully initialized,
        // false otherwise. The functions in MPhysics will always do their
        // own checking for a valid PhysX scene, but other application code
        // may also want to know if physics is available (to control UI
        // elements, for instance).
        __property bool get_Present();

        // Return true if the scene is present
        __property bool get_ScenePresent();

        // Properties. Get functions.
        __property float get_GravityX(); // Gravity components
        __property float get_GravityY();
        __property float get_GravityZ();
        __property float get_TimeStepLength(); // Step length of simulation
        __property bool get_DebugState(); // Debug on or off
        __property float get_DebugScale(); // Debug scale factor
        __property float get_GroundFriction(); // Ground plane friction
        __property float get_GroundBounce(); // Ground plane restitution
        __property float get_ForceExponent();
        __property bool get_NeedToSave();
        __property ArrayList* get_PhysicalSequences();

        // Properties. Set functions.
        __property void set_GravityX(float fX);
        __property void set_GravityY(float fY);
        __property void set_GravityZ(float fZ);
        __property void set_TimeStepLength(float fStep);
        __property void set_DebugState(bool bState);
        __property void set_DebugScale(float fScale);
        __property void set_GroundFriction(float fValue);
        __property void set_GroundBounce(float fValue);
        __property void set_ForceExponent(float fExp);
        __property void set_NeedToSave(bool bNeedToSave);

        // Adds a sequence to physics control. This will add the sequence
        // to the list of managed sequences and to the NiPhysXSequenceManager
        // object. It will also do any set up required for management, and
        // send out an event. Returns true if the sequence was successfully
        // added.
        bool AddSequence(MSequence* pkSequence);
        bool RemoveSequence(MSequence* pkSequence);
        bool IsSequencePhysical(MSequence* pkSequence);
        bool IsSequenceGroupPhysical(MSequenceGroup* pkSeqGroup);

        // Reset the internal physics time, but do not reset the state.
        void ResetTime(float fT);

    protected:
        // Properties
        ArrayList* m_aSequences; // Sequences that are physical
        float m_fGravityX;
        float m_fGravityY;
        float m_fGravityZ;
        float m_fTimestep;
        bool m_bDebugState;
        float m_fDebugScale;
        float m_fGroundFriction;
        float m_fGroundBounce;
        float m_fForceExponent;
        bool m_bNeedToSave;

#if defined(EE_PHYSX_BUILD)
        // These things only defined if PhysX support is built in.
       
    public:
        // Events.
        __event void OnAddPhysicsSequence(MSequence* pkSequence);
        __event void OnRemovePhysicsSequence(MSequence* pkSequence);
        
        // Initiates the physics system. This includes initializing
        // the SDK. This function returns true if the
        // physics was successfully initialized or false otherwise.
        bool Init();

        // Cleans up and releases all physics objects. Releases the
        // scene and SDK.
        void Shutdown();

        // Deletes anything we're holding
        void DeleteContents();
        
        // Update is available in three phases for maximum flexibility.
        void UpdateSources(float fT);
        void SimulateStep(float fT);
        void UpdateDestinations(float fT);

        // Resets the physics system back to its initial conditions. The
        // only argument is the time to which the simulation fetch time
        // should be set. This is the start time for Update calls that follow
        // this call. Does nothing if PhysX is not built.
        // Not all physics object need support this kind of reset, in
        // which case they will remain in their existing state.
        void ResetPhysics(const NiFixedString& kStateName, float fT);      
        
        // Called when a new NIF file is loaded, this function finds all the
        // physics content in the file and adds it to the scene so that it
        // will be simulated. The function should be called after the actor
        // manager has operated on the file, so that it can add the controller
        // manager to the NiPhysXSequenceManager object.
        bool ProcessStream(NiStream* pkStream);
        
        // Called when all the sequences in an actor manager have been set up,
        // or when the NIF file has been reloaded, this sets up the sequences
        // for physics control.
        bool AttachPhysics(NiControllerManager* pkController);

        // Called when an individual KF file is loaded and the sequences in it
        // need to be loaded separately. Assumes AttachPhysics has been called
        bool AddKeySequence(NiSequenceData* pkSeqData);

        // Creates a ground plane object. Raises an event, or does nothing if
        // the plane already exists. Return value is true if the plane was
        // added.
        bool AddGroundPlane();

        // Removes the ground plane. Raises an event. Returns true if the
        // plane was removed. False if was already removed or something
        // went wrong.
        bool RemoveGroundPlane();

        // Adjust the height of the ground plane. This copies the state from
        // the ground plane object in the scene.
        bool AdjustGroundPlane();

        // Initiates a force recording sequence. This clears any existing
        // data. A ray in world space is given, and this is intersected with
        // the physics scene to find an actor that is hit and its hit point.
        // If an actor is hit, returns true, otherwise false.
        bool InitiateForces(NiPoint3& kOrigin, NiPoint3& kDir); 
    
        // Records a force at a given time and magnitude, and applies
        // it to the actor. Input is the world space force.
        void AddForce(float fT, NiPoint3& kForce);
        
        // Finish off a force sequence.
        void FinishForce();
        
        // Given a time, interpolates the forces for that time and
        // applies the resulting force to the actor. If the time is
        // before or after the range of forces stored, then the force is
        // zero. Returns true if a force was applied.
        bool ApplyForce(float fT);

        // Clears all recorded data and sets the actor to 0.
        void ClearForces();
        
        // Reset forces, which moves everything back to the start time
        void ResetForces();
        
    protected:
        __gc class Force {
        public:
            Force(float fT, const NiPoint3& kForce);
            
            // Time for this force
            float m_fT;
            
            // Force in world coordinates
            float m_fForceX;
            float m_fForceY;
            float m_fForceZ;
        };

        __gc class ForceSet {
        public:
            ForceSet(NxActor* pkActor, NxCloth* pkCloth,
                const float fPointX, const float fPointY, const float fPointZ,
                const unsigned int uiVertex);
            ~ForceSet();
            
            void AddForce(float fT, NiPoint3& kForce);
            
            void TruncateStart(const float fT);
            void TruncateEnd(const float fT);
            ForceSet* Split(const float fLowT, const float fHighT);
            
            bool ApplyForce(float fT);
            void ApplyForceToObject(NxVec3& kForce);
            
            // Forces, all of type Force
            ArrayList* m_pkForces;
            
            // Start and end times for this set
            float m_fStart;
            float m_fEnd;

            // Actor or cloth to which we are applying the force
            NxActor* m_pkActor;
            NxCloth* m_pkCloth;

            // Point or vertex at which we are applying the force.
            float m_fPointX;
            float m_fPointY;
            float m_fPointZ;
            unsigned int m_uiVertex;

            // Assumed that forces are added in strictly increasing order.
            
            // Forces will also be used up in increasing order
            unsigned int m_uiLastForceUsed;
        };
        
        __nogc class OutputStream :
            public NxUserOutputStream, public NiMemObject {
            virtual void reportError(NxErrorCode code, const char *message,
                const char *file, int line); 
            virtual NxAssertResponse reportAssertViolation(const char *message,
                const char *file, int line);
            virtual void print (const char *message);    
        };

        static const float FORCE_SPRING_CONST = 0.1f;

        // The hard part of creating all the physics content from a stream.
        bool CreatePhysX(NiTObjectArray<NiPhysXPropPtr>& kProps);
        void TraverseProps(NiTObjectArray<NiPhysXPropPtr>& kProps,
            bool& bHaveFluids, bool& bNeedHardware);
        
        // Helper functions for ground plane
        bool CreateGroundActor();
        void RemoveGroundActor();
    
        // PhysX scene. This is only creates when the stream is processed
        // because we require knowledge of the types of things in the scene
        // in order to know how to set up the scene.
        NiPhysXScene* m_spPhysicsScene;
        
        // Manager for PhysX actor
        NiPhysXSequenceManager* m_pkPhysicsActor;

        // Ground plane actor and source     
        NxActor* m_pkGroundActor;
        NiPhysXDynamicSrc* m_spGroundActorSrc;

        // Stored forces and times.
        ArrayList* m_pkForceSets; // All of type ForceSet
        
        // Information about most recent force sequence, for adding forces
        ForceSet* m_pkForceTarget;
        
        // We assume that forces are added and used essentially in sequence.
        // So these keep track of where we should start looking for a certain
        // time in the force array.
        unsigned int m_uiLastForceSetUsed;
        
        // Last start time - the time we were reset to.
        float m_fResetTime;
        
        // We need a way to block on the simulation step finishing. This
        // critical section controls access to the simulate/FetchResults
        // block. Threads must not be holding the PhysX SDK lock in
        // NiPhysXManager at the time they try to get this lock.
        NiCriticalSection* m_pkSimFetchSection;

        // For debug visualization.
        NiNode* m_pkDebugParent;
        
        // Output logger
        OutputStream* m_pkOutputObject;
        
#endif // if defined(EE_PHYSX_BUILD)
    };
}

#pragma warning(pop)
