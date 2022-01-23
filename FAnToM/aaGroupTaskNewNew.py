### FAnToM Session
### API Version: 20170511
### Used core version:    e1e36b16181506379a22e4b161dbe60a26dd35af (VisPraktikum2021)
### Used toolbox version: fa980e33e3fbc76c9837a4ab19ea1a29a3c21f7f (devel)

################################################################
###                  Reset GUI                               ###
################################################################
fantom.ui.setCamera( 0, fantom.ui.Camera( fantom.math.Vector3(-7.52652, 4.8392, 10.9608), fantom.math.Vector3(1.21861, 3.24672, 0.525773), fantom.math.Vector3(0.100446, 0.992663, -0.06731), 1, 1.0472 ) )
fantom.ui.setCamera( 1, fantom.ui.Camera( fantom.math.Vector3(5.7253, 0, 0), fantom.math.Vector3(0, 0, 0), fantom.math.Vector3(0, 0, 1), 0, 1.0472 ) )
fantom.ui.setCamera( 2, fantom.ui.Camera( fantom.math.Vector3(0, -5.7253, 0), fantom.math.Vector3(0, -1.43051e-06, 0), fantom.math.Vector3(0, 0, 1), 0, 1.0472 ) )
fantom.ui.setCamera( 3, fantom.ui.Camera( fantom.math.Vector3(0, 0, 5.7253), fantom.math.Vector3(0, 0, 0), fantom.math.Vector3(0, 1, 0), 0, 1.0472 ) )

fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 0, fantom.math.Vector4( 1, 0, 0, 1 ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 1, fantom.math.Vector4( -1, 0, 0, 1 ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 2, fantom.math.Vector4( 0, 1, 0, 1 ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 3, fantom.math.Vector4( 0, -1, 0, 1 ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 4, fantom.math.Vector4( 0, 0, 1, 1 ), False ) )
fantom.ui.setClippingPlane( fantom.ui.ClippingPlane( 5, fantom.math.Vector4( -0, 0, -1, 1 ), False ) )

fantom.ui.setBackgroundColor( fantom.math.Color(1, 1, 1, 1) )

fantom.ui.setRotationCenter( fantom.ui.RotationCenter( fantom.math.Vector3(0, 0, 0), True, True, True ) )


################################################################
###                  Create algorithms                       ###
################################################################
Load_VTK = fantom.makeAlgorithm("Load/VTK")
Load_VTK.setName("Load/VTK")
Load_VTK.setAutoSchedule(True)
Load_VTK.setOption("Input File", "/home/thilo/Documents/wissVis/TestData/streamTest2.vtk")
Load_VTK.setOption("Big Endian", True)
Load_VTK.setOption("Dimension", "2D if third component is zero")
Load_VTK.setOption("Time List", "")
fantom.ui.setAlgorithmPosition(Load_VTK, fantom.math.Vector2(0, 35))

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
fantom.ui.setAlgorithmPosition(Grid_ShowGrid, fantom.math.Vector2(0, 274.8))
Grid_ShowGrid.setVisualOutputVisible('Grid', True)

# Inbound connections of this algorithm:
Load_VTK.connect("Grid", Grid_ShowGrid, "Grid")

# Run the algorithm
Grid_ShowGrid.runBlocking()

Tasks_GroupTaskNewNew = fantom.makeAlgorithm("Tasks/GroupTaskNewNew")
Tasks_GroupTaskNewNew.setName("Tasks/GroupTaskNewNew")
Tasks_GroupTaskNewNew.setAutoSchedule(False)
Tasks_GroupTaskNewNew.setOption("sx", -4)
Tasks_GroupTaskNewNew.setOption("sy", 1)
Tasks_GroupTaskNewNew.setOption("sz", 1e-05)
Tasks_GroupTaskNewNew.setOption("ex", -4)
Tasks_GroupTaskNewNew.setOption("ey", 1)
Tasks_GroupTaskNewNew.setOption("ez", 7)
Tasks_GroupTaskNewNew.setOption("Method", "Runge-Kutta")
Tasks_GroupTaskNewNew.setOption("dStep", 0.05)
Tasks_GroupTaskNewNew.setOption("adStep", 0.02)
Tasks_GroupTaskNewNew.setOption("nStep", 1000)
Tasks_GroupTaskNewNew.setOption("colorStartLine", fantom.math.Color(1, 1, 0, 1))
Tasks_GroupTaskNewNew.setOption("colorStream", fantom.math.Color(1, 0, 0, 1))
Tasks_GroupTaskNewNew.setOption("colorSurface", fantom.math.Color(0, 1, 0, 1))
fantom.ui.setAlgorithmPosition(Tasks_GroupTaskNewNew, fantom.math.Vector2(0, 144.4))
Tasks_GroupTaskNewNew.setVisualOutputVisible('surface', True)
Tasks_GroupTaskNewNew.setVisualOutputVisible('streamlines', True)
Tasks_GroupTaskNewNew.setVisualOutputVisible('startline', True)

# Inbound connections of this algorithm:
Load_VTK.connect("Fields", Tasks_GroupTaskNewNew, "Field")

# Run the algorithm
Tasks_GroupTaskNewNew.runBlocking()



