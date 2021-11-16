### FAnToM Session
### API Version: 20170511
### Used core version:    e1e36b16181506379a22e4b161dbe60a26dd35af (VisPraktikum2021)
### Used toolbox version: fa980e33e3fbc76c9837a4ab19ea1a29a3c21f7f (devel)

################################################################
###                  Reset GUI                               ###
################################################################
fantom.ui.setCamera( 0, fantom.ui.Camera( fantom.math.Vector3(9.49728, 1.25694, 4.78729), fantom.math.Vector3(-1.12067, -0.0493129, -0.954852), fantom.math.Vector3(-0.0917472, 0.994178, -0.0565087), 1, 1.0472 ) )
fantom.ui.setCamera( 1, fantom.ui.Camera( fantom.math.Vector3(12.1415, 0, 0), fantom.math.Vector3(0, 0, 0), fantom.math.Vector3(0, 0, 1), 0, 1.0472 ) )
fantom.ui.setCamera( 2, fantom.ui.Camera( fantom.math.Vector3(0, -12.1415, 0), fantom.math.Vector3(0, -6.67572e-06, 0), fantom.math.Vector3(0, 0, 1), 0, 1.0472 ) )
fantom.ui.setCamera( 3, fantom.ui.Camera( fantom.math.Vector3(0, 0, 12.1415), fantom.math.Vector3(0, 0, 0), fantom.math.Vector3(0, 1, 0), 0, 1.0472 ) )

fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 0, fantom.math.Vector4( 1, 0, 0, 1 ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 1, fantom.math.Vector4( -1, 0, 0, 1 ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 2, fantom.math.Vector4( 0, 1, 0, 1 ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 3, fantom.math.Vector4( 0, -1, 0, 1 ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 4, fantom.math.Vector4( 0, 0, 1, 1 ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 5, fantom.math.Vector4( -0, 0, -1, 1 ), False ) )

fantom.ui.setBackgroundColor( fantom.math.Color(0, 0, 0, 1) )

fantom.ui.setRotationCenter( fantom.ui.RotationCenter( fantom.math.Vector3(-0.0395294, 0, 0.828563), True, True, True ) )


################################################################
###                  Create algorithms                       ###
################################################################
Tasks_TaskTest = fantom.makeAlgorithm("Tasks/TaskTest")
Tasks_TaskTest.setName("Tasks/TaskTest")
Tasks_TaskTest.setAutoSchedule(True)
Tasks_TaskTest.setOption("nHouses", 1)
Tasks_TaskTest.setOption("nlevels", 5)
fantom.ui.setAlgorithmPosition(Tasks_TaskTest, fantom.math.Vector2(0, 218.4))

# Inbound connections of this algorithm:

# Run the algorithm
Tasks_TaskTest.runBlocking()

Grid_ShowGrid = fantom.makeAlgorithm("Grid/Show Grid")
Grid_ShowGrid.setName("Grid/Show Grid")
Grid_ShowGrid.setAutoSchedule(True)
Grid_ShowGrid.setOption("Line color", fantom.math.Color(0, 0, 1, 1))
Grid_ShowGrid.setOption("Line width", 1)
Grid_ShowGrid.setOption("Random jittering of color", True)
Grid_ShowGrid.setOption("Random seed", 0)
fantom.ui.setAlgorithmPosition(Grid_ShowGrid, fantom.math.Vector2(0, 130))
Grid_ShowGrid.setVisualOutputVisible('Grid', True)

# Inbound connections of this algorithm:
Tasks_TaskTest.connect("settlement", Grid_ShowGrid, "Grid")

# Run the algorithm
Grid_ShowGrid.runBlocking()



