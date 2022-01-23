### FAnToM Session
### API Version: 20170511
### Used core version:    e1e36b16181506379a22e4b161dbe60a26dd35af (VisPraktikum2021)
### Used toolbox version: fa980e33e3fbc76c9837a4ab19ea1a29a3c21f7f (devel)

################################################################
###                  Reset GUI                               ###
################################################################
fantom.ui.setCamera( 0, fantom.ui.Camera( fantom.math.Vector3(-2.24102, 5.62649, 10.5823), fantom.math.Vector3(1.4213, 0.782032, 0.13586), fantom.math.Vector3(0.0680493, 0.913987, -0.399998), 1, 1.0472 ) )
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

Tasks_GTGrid = fantom.makeAlgorithm("Tasks/GTGrid")
Tasks_GTGrid.setName("Tasks/GTGrid")
Tasks_GTGrid.setAutoSchedule(True)
Tasks_GTGrid.setOption("Surface", "Yes")
Tasks_GTGrid.setOption("ox", -4)
Tasks_GTGrid.setOption("oy", 1)
Tasks_GTGrid.setOption("oz", 1)
Tasks_GTGrid.setOption("nx", 1)
Tasks_GTGrid.setOption("ny", 1)
Tasks_GTGrid.setOption("nz", 5)
Tasks_GTGrid.setOption("dx", 1)
Tasks_GTGrid.setOption("dy", 1)
Tasks_GTGrid.setOption("dz", 1)
Tasks_GTGrid.setOption("Method", "Runge-Kutta")
Tasks_GTGrid.setOption("dStep", 0.05)
Tasks_GTGrid.setOption("adStep", 0.02)
Tasks_GTGrid.setOption("nStep", 100)
Tasks_GTGrid.setOption("colorGrid", fantom.math.Color(1, 1, 1, 1))
Tasks_GTGrid.setOption("colorStream", fantom.math.Color(1, 0, 0, 1))
fantom.ui.setAlgorithmPosition(Tasks_GTGrid, fantom.math.Vector2(0, 144.4))
Tasks_GTGrid.setVisualOutputVisible('surface', True)
Tasks_GTGrid.setVisualOutputVisible('grid', True)
Tasks_GTGrid.setVisualOutputVisible('streams', True)

# Inbound connections of this algorithm:
Load_VTK.connect("Fields", Tasks_GTGrid, "Field")

# Run the algorithm
Tasks_GTGrid.runBlocking()



