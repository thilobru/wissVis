### FAnToM Session
### API Version: 20170511
### Used core version:    e1e36b16181506379a22e4b161dbe60a26dd35af (VisPraktikum2021)
### Used toolbox version: fa980e33e3fbc76c9837a4ab19ea1a29a3c21f7f (devel)

################################################################
###                  Reset GUI                               ###
################################################################
fantom.ui.setCamera( 0, fantom.ui.Camera( fantom.math.Vector3(-2.24102, 5.62649, 10.5823), fantom.math.Vector3(1.4213, 0.782032, 0.135861), fantom.math.Vector3(0.0680488, 0.913987, -0.399996), 1, 1.0472 ) )
fantom.ui.setCamera( 1, fantom.ui.Camera( fantom.math.Vector3(5.7253, 0, 0), fantom.math.Vector3(0, 0, 0), fantom.math.Vector3(0, 0, 1), 0, 1.0472 ) )
fantom.ui.setCamera( 2, fantom.ui.Camera( fantom.math.Vector3(0, -5.7253, 0), fantom.math.Vector3(0, -9.53674e-07, 0), fantom.math.Vector3(0, 0, 1), 0, 1.0472 ) )
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

Tasks_GroupTaskWithGrid = fantom.makeAlgorithm("Tasks/GroupTaskWithGrid")
Tasks_GroupTaskWithGrid.setName("Tasks/GroupTaskWithGrid")
Tasks_GroupTaskWithGrid.setAutoSchedule(True)
Tasks_GroupTaskWithGrid.setOption("Surface", "Yes")
Tasks_GroupTaskWithGrid.setOption("ox", -4)
Tasks_GroupTaskWithGrid.setOption("oy", 1)
Tasks_GroupTaskWithGrid.setOption("oz", 1)
Tasks_GroupTaskWithGrid.setOption("nx", 1)
Tasks_GroupTaskWithGrid.setOption("ny", 1)
Tasks_GroupTaskWithGrid.setOption("nz", 5)
Tasks_GroupTaskWithGrid.setOption("dx", 1)
Tasks_GroupTaskWithGrid.setOption("dy", 1)
Tasks_GroupTaskWithGrid.setOption("dz", 1)
Tasks_GroupTaskWithGrid.setOption("Method", "Runge-Kutta")
Tasks_GroupTaskWithGrid.setOption("dStep", 0.05)
Tasks_GroupTaskWithGrid.setOption("adStep", 0.02)
Tasks_GroupTaskWithGrid.setOption("nStep", 100)
Tasks_GroupTaskWithGrid.setOption("colorGrid", fantom.math.Color(1, 1, 1, 1))
Tasks_GroupTaskWithGrid.setOption("colorStream", fantom.math.Color(1, 0, 0, 1))
fantom.ui.setAlgorithmPosition(Tasks_GroupTaskWithGrid, fantom.math.Vector2(0, 144.4))
Tasks_GroupTaskWithGrid.setVisualOutputVisible('surface', True)
Tasks_GroupTaskWithGrid.setVisualOutputVisible('grid', True)
Tasks_GroupTaskWithGrid.setVisualOutputVisible('streams', True)

# Inbound connections of this algorithm:
Load_VTK.connect("Fields", Tasks_GroupTaskWithGrid, "Field")

# Run the algorithm
Tasks_GroupTaskWithGrid.runBlocking()



