### FAnToM Session
### API Version: 20170511
### Used core version:    e1e36b16181506379a22e4b161dbe60a26dd35af (VisPraktikum2021)
### Used toolbox version: fa980e33e3fbc76c9837a4ab19ea1a29a3c21f7f (devel)

################################################################
###                  Reset GUI                               ###
################################################################
fantom.ui.setCamera( 0, fantom.ui.Camera( fantom.math.Vector3(-5.34432, 5.48547, 9.48751), fantom.math.Vector3(1.29174, 0.843658, 0.519371), fantom.math.Vector3(0.137075, 0.91747, -0.373442), 1, 1.0472 ) )
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

Tasks_GroupTaskWithStartline = fantom.makeAlgorithm("Tasks/GroupTaskWithStartline")
Tasks_GroupTaskWithStartline.setName("Tasks/GroupTaskWithStartline")
Tasks_GroupTaskWithStartline.setAutoSchedule(True)
Tasks_GroupTaskWithStartline.setOption("sx", -4)
Tasks_GroupTaskWithStartline.setOption("sy", 1)
Tasks_GroupTaskWithStartline.setOption("sz", 1)
Tasks_GroupTaskWithStartline.setOption("ex", -4)
Tasks_GroupTaskWithStartline.setOption("ey", 1)
Tasks_GroupTaskWithStartline.setOption("ez", 7)
Tasks_GroupTaskWithStartline.setOption("Method", "Runge-Kutta")
Tasks_GroupTaskWithStartline.setOption("dStep", 0.05)
Tasks_GroupTaskWithStartline.setOption("adStep", 0.02)
Tasks_GroupTaskWithStartline.setOption("nStep", 100)
Tasks_GroupTaskWithStartline.setOption("colorStartLine", fantom.math.Color(1, 1, 0, 1))
Tasks_GroupTaskWithStartline.setOption("colorStream", fantom.math.Color(1, 0, 0, 1))
Tasks_GroupTaskWithStartline.setOption("colorSurface", fantom.math.Color(0, 1, 0, 1))
fantom.ui.setAlgorithmPosition(Tasks_GroupTaskWithStartline, fantom.math.Vector2(0, 144.4))
Tasks_GroupTaskWithStartline.setVisualOutputVisible('surface', True)
Tasks_GroupTaskWithStartline.setVisualOutputVisible('streamlines', True)
Tasks_GroupTaskWithStartline.setVisualOutputVisible('startline', True)

# Inbound connections of this algorithm:
Load_VTK.connect("Fields", Tasks_GroupTaskWithStartline, "Field")

# Run the algorithm
Tasks_GroupTaskWithStartline.runBlocking()



