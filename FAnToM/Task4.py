### FAnToM Session
### API Version: 20170511
### Used core version:    e1e36b16181506379a22e4b161dbe60a26dd35af (VisPraktikum2021)
### Used toolbox version: fa980e33e3fbc76c9837a4ab19ea1a29a3c21f7f (devel)

################################################################
###                  Reset GUI                               ###
################################################################
fantom.ui.setCamera( 0, fantom.ui.Camera( fantom.math.Vector3(-7.01666, 4.76057, 12.6257), fantom.math.Vector3(2.05866, 1.55379, 1.19877), fantom.math.Vector3(0.129242, 0.97668, -0.171444), 1, 1.0472 ) )
fantom.ui.setCamera( 1, fantom.ui.Camera( fantom.math.Vector3(2.08293, 0, 0), fantom.math.Vector3(0, 0, 0), fantom.math.Vector3(0, 0, 1), 0, 1.0472 ) )
fantom.ui.setCamera( 2, fantom.ui.Camera( fantom.math.Vector3(0, -2.08293, 0), fantom.math.Vector3(0, -9.53674e-07, 0), fantom.math.Vector3(0, 0, 1), 0, 1.0472 ) )
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
Tasks_Task4.setOption("dStep", 0.03)
Tasks_Task4.setOption("adStep", 0.02)
Tasks_Task4.setOption("nStep", 100)
Tasks_Task4.setOption("colorGrid", fantom.math.Color(1, 1, 1, 1))
Tasks_Task4.setOption("colorStream", fantom.math.Color(1, 0, 0, 1))
fantom.ui.setAlgorithmPosition(Tasks_Task4, fantom.math.Vector2(0, 35))
Tasks_Task4.setVisualOutputVisible('grid', True)
Tasks_Task4.setVisualOutputVisible('streams', True)

# Inbound connections of this algorithm:
Load_VTK.connect("Fields", Tasks_Task4, "Field")

# Run the algorithm
Tasks_Task4.runBlocking()

Grid_ShowGrid = fantom.makeAlgorithm("Grid/Show Grid")
Grid_ShowGrid.setName("Grid/Show Grid")
Grid_ShowGrid.setAutoSchedule(True)
Grid_ShowGrid.setOption("Line color", fantom.math.Color(0, 0, 1, 1))
Grid_ShowGrid.setOption("Line width", 1)
Grid_ShowGrid.setOption("Random jittering of color", True)
Grid_ShowGrid.setOption("Random seed", 0)
fantom.ui.setAlgorithmPosition(Grid_ShowGrid, fantom.math.Vector2(144.134, 35))
Grid_ShowGrid.setVisualOutputVisible('Grid', True)

# Inbound connections of this algorithm:
Load_VTK.connect("Grid", Grid_ShowGrid, "Grid")

# Run the algorithm
Grid_ShowGrid.runBlocking()



