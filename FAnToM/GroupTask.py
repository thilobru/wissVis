### FAnToM Session
### API Version: 20170511
### Used core version:    e1e36b16181506379a22e4b161dbe60a26dd35af (VisPraktikum2021)
### Used toolbox version: fa980e33e3fbc76c9837a4ab19ea1a29a3c21f7f (devel)

################################################################
###                  Reset GUI                               ###
################################################################
fantom.ui.setCamera( 0, fantom.ui.Camera( fantom.math.Vector3(-5.26547, 7.90871, 7.3106), fantom.math.Vector3(1.87987, 1.49292, 0.570423), fantom.math.Vector3(0.434272, 0.836011, -0.335399), 1, 1.0472 ) )
fantom.ui.setCamera( 1, fantom.ui.Camera( fantom.math.Vector3(5.7253, 0, 0), fantom.math.Vector3(0, 0, 0), fantom.math.Vector3(0, 0, 1), 0, 1.0472 ) )
fantom.ui.setCamera( 2, fantom.ui.Camera( fantom.math.Vector3(0, -5.7253, 0), fantom.math.Vector3(0, -4.29153e-06, 0), fantom.math.Vector3(0, 0, 1), 0, 1.0472 ) )
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
fantom.ui.setAlgorithmPosition(Grid_ShowGrid, fantom.math.Vector2(0, 144.4))
Grid_ShowGrid.setVisualOutputVisible('Grid', True)

# Inbound connections of this algorithm:
Load_VTK.connect("Grid", Grid_ShowGrid, "Grid")

# Run the algorithm
Grid_ShowGrid.runBlocking()

Tasks_GroupTask = fantom.makeAlgorithm("Tasks/GroupTask")
Tasks_GroupTask.setName("Tasks/GroupTask")
Tasks_GroupTask.setAutoSchedule(False)
Tasks_GroupTask.setOption("Surface", "Yes")
Tasks_GroupTask.setOption("ox", -4)
Tasks_GroupTask.setOption("oy", 1)
Tasks_GroupTask.setOption("oz", 0.001)
Tasks_GroupTask.setOption("nx", 1)
Tasks_GroupTask.setOption("ny", 1)
Tasks_GroupTask.setOption("nz", 200)
Tasks_GroupTask.setOption("dx", 1)
Tasks_GroupTask.setOption("dy", 1)
Tasks_GroupTask.setOption("dz", 0.01)
Tasks_GroupTask.setOption("Method", "Runge-Kutta")
Tasks_GroupTask.setOption("dStep", 0.05)
Tasks_GroupTask.setOption("adStep", 0.02)
Tasks_GroupTask.setOption("nStep", 500)
Tasks_GroupTask.setOption("colorGrid", fantom.math.Color(1, 1, 1, 1))
Tasks_GroupTask.setOption("colorStream", fantom.math.Color(1, 0, 0, 1))
fantom.ui.setAlgorithmPosition(Tasks_GroupTask, fantom.math.Vector2(0, 231.8))
Tasks_GroupTask.setVisualOutputVisible('surface', True)
Tasks_GroupTask.setVisualOutputVisible('grid', True)
Tasks_GroupTask.setVisualOutputVisible('streams', False)

# Inbound connections of this algorithm:
Load_VTK.connect("Fields", Tasks_GroupTask, "Field")

# Run the algorithm
Tasks_GroupTask.runBlocking()



