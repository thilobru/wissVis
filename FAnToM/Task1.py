### FAnToM Session
### API Version: 20170511
### Used core version:    e1e36b16181506379a22e4b161dbe60a26dd35af (VisPraktikum2021)
### Used toolbox version: fa980e33e3fbc76c9837a4ab19ea1a29a3c21f7f (devel)

################################################################
###                  Reset GUI                               ###
################################################################
fantom.ui.setCamera( 0, fantom.ui.Camera( fantom.math.Vector3(-22.4725, 54.9774, 119.705), fantom.math.Vector3(0.232553, 16.2725, -5.50149), fantom.math.Vector3(0.0625567, 0.956666, -0.284389), 1, 1.0472 ) )
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

fantom.ui.setRotationCenter( fantom.ui.RotationCenter( fantom.math.Vector3(1.03628, 0.180553, -0.367794), True, True, True ) )


################################################################
###                  Create algorithms                       ###
################################################################
Tasks_Task1 = fantom.makeAlgorithm("Tasks/Task1")
Tasks_Task1.setName("Tasks/Task1")
Tasks_Task1.setAutoSchedule(True)
Tasks_Task1.setOption("nHouses", 400)
Tasks_Task1.setOption("nlevels", 5)
fantom.ui.setAlgorithmPosition(Tasks_Task1, fantom.math.Vector2(0, 35))

# Inbound connections of this algorithm:

# Run the algorithm
Tasks_Task1.runBlocking()

Grid_ShowGrid = fantom.makeAlgorithm("Grid/Show Grid")
Grid_ShowGrid.setName("Grid/Show Grid")
Grid_ShowGrid.setAutoSchedule(True)
Grid_ShowGrid.setOption("Line color", fantom.math.Color(0, 0, 1, 1))
Grid_ShowGrid.setOption("Line width", 1)
Grid_ShowGrid.setOption("Random jittering of color", True)
Grid_ShowGrid.setOption("Random seed", 0)
fantom.ui.setAlgorithmPosition(Grid_ShowGrid, fantom.math.Vector2(255, 126.4))
Grid_ShowGrid.setVisualOutputVisible('Grid', True)

# Inbound connections of this algorithm:
Tasks_Task1.connect("settlement", Grid_ShowGrid, "Grid")

# Run the algorithm
Grid_ShowGrid.runBlocking()



