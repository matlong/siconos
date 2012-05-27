// Warning: 
// You have to PY_REGISTER base classe before derivated classes
#undef PY_REGISTER
#define KERNEL_REGISTRATION()                                           \
  PY_REGISTER(NonSmoothLaw);                                            \
  PY_REGISTER(NewtonImpactNSL);                                         \
  PY_REGISTER(NewtonImpactFrictionNSL);                                 \
  PY_REGISTER(MixedComplementarityConditionNSL);                        \
  PY_REGISTER(ComplementarityConditionNSL);                             \
  PY_REGISTER(EqualityConditionNSL);                                    \
  PY_REGISTER(MultipleImpactNSL);                                       \
  PY_REGISTER(RelayNSL);                                                \
  PY_REGISTER(DynamicalSystem);                                         \
  PY_REGISTER(NonSmoothDynamicalSystem);                                \
  PY_REGISTER(Topology);                                                \
  PY_REGISTER(LagrangianDS);                                            \
  PY_REGISTER(LagrangianLinearTIDS);                                    \
  PY_REGISTER(NewtonEulerDS);                                           \
  PY_REGISTER(FirstOrderNonLinearDS);                                   \
  PY_REGISTER(FirstOrderLinearDS);                                      \
  PY_REGISTER(FirstOrderLinearTIDS);                                    \
  PY_REGISTER(Relation);                                                \
  PY_REGISTER(LagrangianR);                                             \
  PY_REGISTER(LagrangianLinearTIR);                                     \
  PY_REGISTER(LagrangianRheonomousR);                                   \
  PY_REGISTER(LagrangianScleronomousR);                                 \
  PY_REGISTER(LagrangianCompliantR);                                    \
  PY_REGISTER(NewtonEulerR);                                            \
  PY_REGISTER(NewtonEulerFrom1DLocalFrameR);                            \
  PY_REGISTER(NewtonEulerFrom3DLocalFrameR);                            \
  PY_REGISTER(KneeJointR)                                               \
  PY_REGISTER(PivotJointR)                                              \
  PY_REGISTER(PrismaticJointR)                                          \
  PY_REGISTER(FirstOrderR);                                             \
  PY_REGISTER(FirstOrderLinearR);                                       \
  PY_REGISTER(FirstOrderLinearTIR);                                     \
  PY_REGISTER(FirstOrderType1R);                                        \
  PY_REGISTER(FirstOrderType2R);                                        \
  PY_REGISTER_WITHOUT_DIRECTOR(Actuator)                                \
  PY_REGISTER_WITHOUT_DIRECTOR(Sensor)                                  \
  PY_REGISTER(ControlSensor)                                            \
  PY_REGISTER(LinearSensor)                                             \
  PY_REGISTER(SampledPIDActuator)                                  \
  PY_REGISTER(CommonSMC)                                           \
  PY_REGISTER(LinearSMC)                                           \
  PY_REGISTER(LinearChatteringSMC)                                 \
  PY_REGISTER(LinearSMCOT2)                                       \
  PY_REGISTER(ControlManager)                                      \
  PY_REGISTER(ControlDynamicalSystem)                              \
  PY_REGISTER(ControlFirstOrderLinearDS)                           \
  PY_REGISTER(Interaction);                                        \
  PY_REGISTER(TimeDiscretisation);                                 \
  PY_REGISTER(OneStepNSProblem);                                   \
  PY_REGISTER(OneStepIntegrator);                                  \
  PY_REGISTER(LinearOSNS);                                         \
  PY_REGISTER(Lsodar);                                             \
  PY_REGISTER(LCP);                                                \
  PY_REGISTER(QP);                                                 \
  PY_REGISTER(Relay);                                              \
  PY_REGISTER(MLCP);                                               \
  PY_REGISTER(MLCPProjectOnConstraints);                           \
  PY_REGISTER(FrictionContact);                                    \
  PY_REGISTER(Moreau);                                             \
  PY_REGISTER(Simulation);                                         \
  PY_REGISTER(TimeStepping);                                       \
  PY_REGISTER(EventDriven);                                        \
  PY_REGISTER(Model);                                              \
  PY_REGISTER(CircularDS);                                         \
  PY_REGISTER(Disk);                                               \
  PY_REGISTER(Circle);                                             \
  PY_REGISTER(ExternalBody);                                       \
  PY_REGISTER(CircularR);                                          \
  PY_REGISTER(DiskDiskR);                                          \
  PY_REGISTER(DiskPlanR);                                          \
  PY_REGISTER(DiskMovingPlanR);                                    \
  PY_REGISTER(SphereLDS);                                          \
  PY_REGISTER(SphereNEDS);                                         \
  PY_REGISTER(SphereLDSPlanR);                                     \
  PY_REGISTER(SphereNEDSPlanR);                                    \
  PY_REGISTER(SphereLDSSphereLDSR);                                \
  PY_REGISTER(SphereNEDSSphereNEDSR);                              \
  PY_REGISTER(SiconosBodies);                                      \
  PY_REGISTER(SpaceFilter);                                        \
  PY_REGISTER_WITHOUT_DIRECTOR(SiconosMatrix);                     \
  PY_REGISTER_WITHOUT_DIRECTOR(SimpleMatrix);                      \
  PY_REGISTER_WITHOUT_DIRECTOR(SiconosVector);                     \
  PY_REGISTER_WITHOUT_DIRECTOR(SimpleVector);                      \
  PY_REGISTER_WITHOUT_DIRECTOR(BlockVector);                       \
  PY_REGISTER(InteractionsSet);                                    \
  PY_REGISTER(BoundaryCondition);                                  \
  PY_REGISTER(ioMatrix);                                           \
  PY_REGISTER_BULLET_MAYBE();

#ifdef WITH_BULLET
#define PY_REGISTER_BULLET_MAYBE()                                      \
  PY_REGISTER(BulletR);                                                 \
  PY_REGISTER(BulletDS);                                                \
  PY_REGISTER(BulletTimeStepping);                                      \
  PY_REGISTER(BulletTimeSteppingProjectOnConstraints);                  \
  PY_REGISTER(BulletWeightedShape);                                     \
  PY_REGISTER(BulletFrom1DLocalFrameR);                                 \
  PY_REGISTER_BULLET(btCollisionShape)                                  \
  PY_REGISTER_BULLET(btCollisionMargin)                                 \
  PY_REGISTER_BULLET(btConvexShape)                                     \
  PY_REGISTER_BULLET(btConvexInternalShape)                             \
  PY_REGISTER_BULLET(btConvex2dShape)                                   \
  PY_REGISTER_BULLET(btConvexPointCloudShape)                           \
  PY_REGISTER_BULLET(btConvexHullShape)                                 \
  PY_REGISTER_BULLET(btConvexPolyhedron)                                \
  PY_REGISTER_BULLET(btConvexTriangleMeshShape)                         \
  PY_REGISTER_BULLET(btPolyhedralConvexShape)                           \
  PY_REGISTER_BULLET(btConcaveShape)                                    \
  PY_REGISTER_BULLET(btEmptyShape)                                      \
  PY_REGISTER_BULLET(btCompoundShape)                                   \
  PY_REGISTER_BULLET(btTriangleShape)                                   \
  PY_REGISTER_BULLET(btTriangleMesh)                                    \
  PY_REGISTER_BULLET(btTriangleMeshShape)                               \
  PY_REGISTER_BULLET(btBox2dShape)                                      \
  PY_REGISTER_BULLET(btBoxShape)                                        \
  PY_REGISTER_BULLET(btCapsuleShape)                                    \
  PY_REGISTER_BULLET(btConeShape)                                       \
  PY_REGISTER_BULLET(btCylinderShape)                                   \
  PY_REGISTER_BULLET(btHeightfieldTerrainShape)                         \
  PY_REGISTER_BULLET(btMaterial)                                        \
  PY_REGISTER_BULLET(btMinkowskiSumShape)                               \
  PY_REGISTER_BULLET(btSphereShape)                                     \
  PY_REGISTER_BULLET(btMultiSphereShape)                                \
  PY_REGISTER_BULLET(btMultimaterialTriangleMeshShape)                  \
  PY_REGISTER_BULLET(btOptimizedBvh)                                    \
  PY_REGISTER_BULLET(btScaledBvhTriangleMeshShape)                      \
  PY_REGISTER_BULLET(btShapeHull)                                       \
  PY_REGISTER_BULLET(btStaticPlaneShape)                                \
  PY_REGISTER_BULLET(btStridingMeshInterface)                           \
  PY_REGISTER_BULLET(btTetrahedronShape)                                \
  PY_REGISTER_BULLET(btTriangleBuffer)                                  \
  PY_REGISTER_BULLET(btTriangleCallback)                                \
  PY_REGISTER_BULLET(btTriangleIndexVertexArray)                        \
  PY_REGISTER_BULLET(btTriangleIndexVertexMaterialArray)                \
  PY_REGISTER_BULLET(btTriangleInfoMap)                                 \
  PY_REGISTER_BULLET(btUniformScalingShape)
#else
#define PY_REGISTER_BULLET_MAYBE()
#endif
