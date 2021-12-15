### FAnToM Session
### API Version: 20170511
### Used core version:    e1e36b16181506379a22e4b161dbe60a26dd35af (VisPraktikum2021)
### Used toolbox version: fa980e33e3fbc76c9837a4ab19ea1a29a3c21f7f (devel)

################################################################
###                  Reset GUI                               ###
################################################################
fantom.ui.setCamera( 0, fantom.ui.Camera( fantom.math.Vector3(-6.29861, 7.17992, 11.5736), fantom.math.Vector3(0.939703, 1.03361, 0.038888), fantom.math.Vector3(0.26103, 0.910306, -0.321257), 1, 1.0472 ) )
fantom.ui.setCamera( 1, fantom.ui.Camera( fantom.math.Vector3(2.08293, 0, 0), fantom.math.Vector3(0, 0, 0), fantom.math.Vector3(0, 0, 1), 0, 1.0472 ) )
fantom.ui.setCamera( 2, fantom.ui.Camera( fantom.math.Vector3(0, -2.08293, 0), fantom.math.Vector3(0, -1.66893e-06, 0), fantom.math.Vector3(0, 0, 1), 0, 1.0472 ) )
fantom.ui.setCamera( 3, fantom.ui.Camera( fantom.math.Vector3(0.140779, 2.86971, 7.02662), fantom.math.Vector3(0.140779, 2.86971, 0), fantom.math.Vector3(0, 1, 0), 0, 1.0472 ) )

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
Load_VTK = fantom.makeAlgorithm("Load/VTK")
Load_VTK.setName("Load/VTK")
Load_VTK.setAutoSchedule(True)
Load_VTK.setOption("Input File", "/home/thilo/Documents/wissVis/TestData/streamTest2.vtk")
Load_VTK.setOption("Big Endian", True)
Load_VTK.setOption("Dimension", "3D")
Load_VTK.setOption("Time List", "")
fantom.ui.setAlgorithmPosition(Load_VTK, fantom.math.Vector2(6.5625, -51.2))

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
fantom.ui.setAlgorithmPosition(Grid_ShowGrid, fantom.math.Vector2(214.134, 207))
Grid_ShowGrid.setVisualOutputVisible('Grid', True)

# Inbound connections of this algorithm:
Load_VTK.connect("Grid", Grid_ShowGrid, "Grid")

# Run the algorithm
Grid_ShowGrid.runBlocking()

Tasks_Task4 = fantom.makeAlgorithm("Tasks/Task4")
Tasks_Task4.setName("Tasks/Task4")
Tasks_Task4.setAutoSchedule(True)
Tasks_Task4.setOption("ox", -4)
Tasks_Task4.setOption("oy", 0.5)
Tasks_Task4.setOption("oz", 1)
Tasks_Task4.setOption("nx", 1)
Tasks_Task4.setOption("ny", 7)
Tasks_Task4.setOption("nz", 5)
Tasks_Task4.setOption("dx", 1)
Tasks_Task4.setOption("dy", 1)
Tasks_Task4.setOption("dz", 1)
Tasks_Task4.setOption("Method", "Euler")
Tasks_Task4.setOption("dStep", 0.02)
Tasks_Task4.setOption("adStep", 0.02)
Tasks_Task4.setOption("nStep", 200)
Tasks_Task4.setOption("colorGrid", fantom.math.Color(1, 1, 1, 1))
Tasks_Task4.setOption("colorStream", fantom.math.Color(1, 0, 0, 1))
fantom.ui.setAlgorithmPosition(Tasks_Task4, fantom.math.Vector2(1, 161))
Tasks_Task4.setVisualOutputVisible('grid', True)
Tasks_Task4.setVisualOutputVisible('streams', True)

# Inbound connections of this algorithm:
Load_VTK.connect("Fields", Tasks_Task4, "Field")

# Run the algorithm
Tasks_Task4.runBlocking()



