### FAnToM Session
### API Version: 20170511
### Used core version:    e1e36b16181506379a22e4b161dbe60a26dd35af (VisPraktikum2021)
### Used toolbox version: fa980e33e3fbc76c9837a4ab19ea1a29a3c21f7f (devel)

################################################################
###                  Reset GUI                               ###
################################################################
fantom.ui.setCamera( 0, fantom.ui.Camera( fantom.math.Vector3(-2823.85, 189.279, 513.673), fantom.math.Vector3(889.449, 513.039, 489.797), fantom.math.Vector3(-4.17233e-07, 0.0735517, 0.997291), 1, 1.0472 ) )
fantom.ui.setCamera( 1, fantom.ui.Camera( fantom.math.Vector3(5.7253, 0, 0), fantom.math.Vector3(0, 0, 0), fantom.math.Vector3(0, 0, 1), 0, 1.0472 ) )
fantom.ui.setCamera( 2, fantom.ui.Camera( fantom.math.Vector3(0, -5.7253, 0), fantom.math.Vector3(0, -2.38419e-06, 0), fantom.math.Vector3(0, 0, 1), 0, 1.0472 ) )
fantom.ui.setCamera( 3, fantom.ui.Camera( fantom.math.Vector3(0, 0, 5.7253), fantom.math.Vector3(0, 0, 0), fantom.math.Vector3(0, 1, 0), 0, 1.0472 ) )

fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 0, fantom.math.Vector4( 1, 0, 0, 1 ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 1, fantom.math.Vector4( -1, 0, 0, 1 ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 2, fantom.math.Vector4( 0, 1, 0, 1 ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 3, fantom.math.Vector4( 0, -1, 0, 1 ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 4, fantom.math.Vector4( 0, 0, 1, 1 ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 5, fantom.math.Vector4( -0, 0, -1, 1 ), False ) )

fantom.ui.setBackgroundColor( fantom.math.Color(0, 0, 0, 1) )

fantom.ui.setRotationCenter( fantom.ui.RotationCenter( fantom.math.Vector3(710.643, 1250.09, 1876.67), True, True, True ) )


################################################################
###                  Create algorithms                       ###
################################################################
Load_VTK = fantom.makeAlgorithm("Load/VTK")
Load_VTK.setName("Load/VTK")
Load_VTK.setAutoSchedule(True)
Load_VTK.setOption("Input File", "/home/thilo/Documents/wissVis/TestData/markPosVector.vtk")
Load_VTK.setOption("Big Endian", True)
Load_VTK.setOption("Dimension", "2D if third component is zero")
Load_VTK.setOption("Time List", "")
fantom.ui.setAlgorithmPosition(Load_VTK, fantom.math.Vector2(21, 29.4))

# Inbound connections of this algorithm:

# Run the algorithm
Load_VTK.runBlocking()

Grid_ShowGrid = fantom.makeAlgorithm("Grid/Show Grid")
Grid_ShowGrid.setName("Grid/Show Grid")
Grid_ShowGrid.setAutoSchedule(True)
Grid_ShowGrid.setOption("Line color", fantom.math.Color(0, 0, 1, 1))
Grid_ShowGrid.setOption("Line width", 1)
Grid_ShowGrid.setOption("Random jittering of color", True)
Grid_ShowGrid.setOption("Random seed", 0)
fantom.ui.setAlgorithmPosition(Grid_ShowGrid, fantom.math.Vector2(9, 247.4))
Grid_ShowGrid.setVisualOutputVisible('Grid', True)

# Inbound connections of this algorithm:
Load_VTK.connect("Grid", Grid_ShowGrid, "Grid")

# Run the algorithm
Grid_ShowGrid.runBlocking()

Tasks_GTStartlineGradual = fantom.makeAlgorithm("Tasks/GTStartlineGradual")
Tasks_GTStartlineGradual.setName("Tasks/GTStartlineGradual")
Tasks_GTStartlineGradual.setAutoSchedule(False)
Tasks_GTStartlineGradual.setOption("sx", -1000)
Tasks_GTStartlineGradual.setOption("sy", 0)
Tasks_GTStartlineGradual.setOption("sz", 1000)
Tasks_GTStartlineGradual.setOption("ex", -10)
Tasks_GTStartlineGradual.setOption("ey", 0)
Tasks_GTStartlineGradual.setOption("ez", 100)
Tasks_GTStartlineGradual.setOption("Method", "Runge-Kutta")
Tasks_GTStartlineGradual.setOption("dStep", 5)
Tasks_GTStartlineGradual.setOption("adStep", 0.02)
Tasks_GTStartlineGradual.setOption("nStep", 100)
Tasks_GTStartlineGradual.setOption("colorStartLine", fantom.math.Color(1, 1, 0, 1))
Tasks_GTStartlineGradual.setOption("colorStream", fantom.math.Color(1, 0, 0, 1))
Tasks_GTStartlineGradual.setOption("colorSurface", fantom.math.Color(0, 1, 0, 1))
fantom.ui.setAlgorithmPosition(Tasks_GTStartlineGradual, fantom.math.Vector2(157, 131))
Tasks_GTStartlineGradual.setVisualOutputVisible('surface', True)
Tasks_GTStartlineGradual.setVisualOutputVisible('streamlines', True)
Tasks_GTStartlineGradual.setVisualOutputVisible('startline', True)

# Inbound connections of this algorithm:
Load_VTK.connect("Fields", Tasks_GTStartlineGradual, "Field")

# Run the algorithm
Tasks_GTStartlineGradual.runBlocking()



