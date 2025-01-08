#ifndef COLLISION_CB_H
#define COLLISION_CB_H

#include "common.h"

class MyCollisionCallback : public physx::PxSimulationEventCallback {
public:
    MyCollisionCallback() {}
    ~MyCollisionCallback() {}

    void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) override {}
    void onWake(physx::PxActor** actors, physx::PxU32 count) override {}
    void onSleep(physx::PxActor** actors, physx::PxU32 count) override {}

    void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override {
        //std::cout << "on Contact" << std::endl;
        for (physx::PxU32 i = 0; i < nbPairs; i++) {
            const physx::PxContactPair& contact = pairs[i];
            if (contact.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) {
                //std::cout << "eNOTIFY_TOUCH_FOUND" << std::endl;
                physx::PxRigidActor* actorA = static_cast<physx::PxRigidActor*>(pairHeader.actors[0]);
                physx::PxRigidActor* actorB = static_cast<physx::PxRigidActor*>(pairHeader.actors[1]);
                //testActorType(actorA); testActorType(actorB);

                if (kinematicAndStatic(actorA, actorB)) {
					std::cout << "Kinematic touch Static" << std::endl;
                    kinematicTouchStatic = true;
                }
            }
            if (contact.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST) {
                //std::cout << "eNOTIFY_TOUCH_LOST" << std::endl;
                physx::PxRigidActor* actorA = static_cast<physx::PxRigidActor*>(pairHeader.actors[0]);
                physx::PxRigidActor* actorB = static_cast<physx::PxRigidActor*>(pairHeader.actors[1]);
                //testActorType(actorA); testActorType(actorB);

                if (kinematicAndStatic(actorA, actorB)) {
                    std::cout << "Kinematic leave Static" << std::endl;
                    kinematicTouchStatic = false;
                }
            }
        }
    }

    void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override {}
    void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) override {} 

private:
    inline bool isRigidDynamicKinematic(physx::PxRigidActor* actor) {
        if (!actor->is<physx::PxRigidDynamic>()) return false;

        physx::PxRigidDynamic* dyn_actor = static_cast<physx::PxRigidDynamic*>(actor);
        return dyn_actor->getRigidBodyFlags().isSet(physx::PxRigidBodyFlag::eKINEMATIC);
    }
    inline bool isRigidStatic(physx::PxRigidActor* actor) { return actor->is<physx::PxRigidStatic>(); }
    inline bool isRigidDynamic(physx::PxRigidActor* actor) { return actor->is<physx::PxRigidDynamic>(); }
    inline bool kinematicAndStatic(physx::PxRigidActor* actor1, physx::PxRigidActor* actor2) {
        return (isRigidDynamicKinematic(actor1) && isRigidStatic(actor2)) 
            || (isRigidDynamicKinematic(actor2) && isRigidStatic(actor1));
    }

    void testActorType(physx::PxRigidActor* actor) {
        if (isRigidStatic(actor)) {
			std::cout << actor << " : detect RigidStatic" << std::endl;
		}
        else if (isRigidDynamic(actor)) {
			std::cout << actor << " : detect RigidDynamic" << std::endl;
        }
    }
};

#endif // !COLLISION_CB_H
