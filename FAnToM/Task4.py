### FAnToM Session
### API Version: 20170511
### Used core version:    e1e36b16181506379a22e4b161dbe60a26dd35af (VisPraktikum2021)
### Used toolbox version: fa980e33e3fbc76c9837a4ab19ea1a29a3c21f7f (devel)

################################################################
###                  Reset GUI                               ###
################################################################
fantom.ui.setCamera( 0, fantom.ui.Camera( fantom.math.Vector3(-4.3989, 3.49038, 13.6692), fantom.math.Vector3(-1.62688, 2.57324, 2.34882), fantom.math.Vector3(-0.203091, 0.970708, -0.128375), 1, 1.0472 ) )
fantom.ui.setCamera( 1, fantom.ui.Camera( fantom.math.Vector3(4.81755, 0, 0), fantom.math.Vector3(0, 0, 0), fantom.math.Vector3(0, 0, 1), 0, 1.0472 ) )
fantom.ui.setCamera( 2, fantom.ui.Camera( fantom.math.Vector3(0, -4.81755, 0), fantom.math.Vector3(0, -4.76837e-07, 0), fantom.math.Vector3(0, 0, 1), 0, 1.0472 ) )
fantom.ui.setCamera( 3, fantom.ui.Camera( fantom.math.Vector3(0, 0, 4.81755), fantom.math.Vector3(0, 0, 0), fantom.math.Vector3(0, 1, 0), 0, 1.0472 ) )

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
fantom.ui.setAlgorithmPosition(Load_VTK, fantom.math.Vector2(0, 142))

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
fantom.ui.setAlgorithmPosition(Grid_ShowGrid, fantom.math.Vector2(135, 146.5))
Grid_ShowGrid.setVisualOutputVisible('Grid', True)

# Inbound connections of this algorithm:
Load_VTK.connect("Grid", Grid_ShowGrid, "Grid")

# Run the algorithm
Grid_ShowGrid.runBlocking()

Tasks_Task4 = fantom.makeAlgorithm("Tasks/Task4")
Tasks_Task4.setName("Tasks/Task4")
Tasks_Task4.setAutoSchedule(True)
Tasks_Task4.setOption("nx", 5)
Tasks_Task4.setOption("ny", 1)
Tasks_Task4.setOption("nz", 5)
Tasks_Task4.setOption("dx", 1)
Tasks_Task4.setOption("dy", 2)
Tasks_Task4.setOption("dz", 1)
Tasks_Task4.setOption("Step size", 0.05)
Tasks_Task4.setOption("adaptive step size", 0.02)
Tasks_Task4.setOption("Method", "Euler")
Tasks_Task4.setOption("ColorGrid", fantom.math.Color(0.25, 0, 0.25, 1))
Tasks_Task4.setOption("ColorStream", fantom.math.Color(0.5, 0, 0, 1))
Tasks_Task4.setOption("Number of steps", 100)
fantom.ui.setAlgorithmPosition(Tasks_Task4, fantom.math.Vector2(0, 35))
Tasks_Task4.setVisualOutputVisible('Grid', True)
Tasks_Task4.setVisualOutputVisible('Streamlines', True)

# Inbound connections of this algorithm:
Load_VTK.connect("Grid", Tasks_Task4, "Field")

# Run the algorithm
Tasks_Task4.runBlocking()



