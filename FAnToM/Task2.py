### FAnToM Session
### API Version: 20170511
### Used core version:    e1e36b16181506379a22e4b161dbe60a26dd35af (VisPraktikum2021)
### Used toolbox version: fa980e33e3fbc76c9837a4ab19ea1a29a3c21f7f (devel)

################################################################
###                  Reset GUI                               ###
################################################################
fantom.ui.setCamera( 0, fantom.ui.Camera( fantom.math.Vector3(0, 0, 3.14744), fantom.math.Vector3(0, 0, 0), fantom.math.Vector3(0, 1, 0), 1, 1.0472 ) )
fantom.ui.setCamera( 1, fantom.ui.Camera( fantom.math.Vector3(3.14744, 0, 0), fantom.math.Vector3(0, 0, 0), fantom.math.Vector3(0, 0, 1), 0, 1.0472 ) )
fantom.ui.setCamera( 2, fantom.ui.Camera( fantom.math.Vector3(0, -3.14744, 0), fantom.math.Vector3(0, -2.38419e-07, 0), fantom.math.Vector3(0, 0, 1), 0, 1.0472 ) )
fantom.ui.setCamera( 3, fantom.ui.Camera( fantom.math.Vector3(0, 0, 3.14744), fantom.math.Vector3(0, 0, 0), fantom.math.Vector3(0, 1, 0), 0, 1.0472 ) )

fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 0, fantom.math.Vector4( 1, 0, 0, 1 ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 1, fantom.math.Vector4( -1, 0, 0, 1 ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 2, fantom.math.Vector4( 0, 1, 0, 1 ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 3, fantom.math.Vector4( 0, -1, 0, 1 ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 4, fantom.math.Vector4( 0, 0, 1, 1 ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 5, fantom.math.Vector4( -0, 0, -1, 1 ), False ) )

fantom.ui.setBackgroundColor( fantom.math.Color(0, 0, 0, 1) )

fantom.ui.setRotationCenter( fantom.ui.RotationCenter( fantom.math.Vector3(0, 0, 0), True, True, True ) )


################################################################
###                  Create algorithms                       ###
################################################################
Tasks_Task2 = fantom.makeAlgorithm("Tasks/Task2")
Tasks_Task2.setName("Tasks/Task2")
Tasks_Task2.setAutoSchedule(True)
fantom.ui.setAlgorithmPosition(Tasks_Task2, fantom.math.Vector2(0, 352.4))
Tasks_Task2.setVisualOutputVisible('outGrid', True)

# Inbound connections of this algorithm:

# Run the algorithm
Tasks_Task2.runBlocking()

Tasks_TaskFinal = fantom.makeAlgorithm("Tasks/TaskFinal")
Tasks_TaskFinal.setName("Tasks/TaskFinal")
Tasks_TaskFinal.setAutoSchedule(True)
Tasks_TaskFinal.setOption("nHouses", 3)
Tasks_TaskFinal.setOption("nlevels", 5)
fantom.ui.setAlgorithmPosition(Tasks_TaskFinal, fantom.math.Vector2(240, 264))

# Inbound connections of this algorithm:

# Run the algorithm
Tasks_TaskFinal.runBlocking()



