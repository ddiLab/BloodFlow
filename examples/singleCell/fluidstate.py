# script-version: 2.0
# Catalyst state generated using paraview version 5.9.1

#### import the simple module from the paraview
from paraview.simple import *
#### disable automatic camera reset on 'Show'
paraview.simple._DisableFirstRenderCameraReset()

# ----------------------------------------------------------------
# setup views used in the visualization
# ----------------------------------------------------------------

# get the material library
materialLibrary1 = GetMaterialLibrary()

# Create a new 'Render View'
renderView1 = CreateView('RenderView')
renderView1.ViewSize = [1156, 796]
renderView1.AxesGrid = 'GridAxes3DActor'
renderView1.CenterOfRotation = [9.5, 9.5, 19.5]
renderView1.StereoType = 'Crystal Eyes'
renderView1.CameraPosition = [62.693849091880274, 35.73091646520136, 89.16594147474711]
renderView1.CameraFocalPoint = [9.5, 9.5, 19.5]
renderView1.CameraViewUp = [-0.07475138314908182, 0.9507235505109627, -0.300893604486915]
renderView1.CameraFocalDisk = 1.0
renderView1.CameraParallelScale = 23.680160472429236
renderView1.BackEnd = 'OSPRay raycaster'
renderView1.OSPRayMaterialLibrary = materialLibrary1

SetActiveView(None)

# ----------------------------------------------------------------
# setup view layouts
# ----------------------------------------------------------------

# create new layout object 'Layout #1'
layout1 = CreateLayout(name='Layout #1')
layout1.AssignView(0, renderView1)
layout1.SetSize(1156, 796)

# ----------------------------------------------------------------
# restore active view
SetActiveView(renderView1)
# ----------------------------------------------------------------

# ----------------------------------------------------------------
# setup the data processing pipelines
# ----------------------------------------------------------------

# create a new 'PVD Reader'
fluidpvd = PVDReader(registrationName='fluid', FileName='C:\\Users\\Connor\\Documents\\post2\\fluid.pvd')
fluidpvd.PointArrays = ['velocityNorm', 'velocity', 'vorticity']

# ----------------------------------------------------------------
# setup the visualization in view 'renderView1'
# ----------------------------------------------------------------

# show data from fluidpvd
fluidpvdDisplay = Show(fluidpvd, renderView1, 'UniformGridRepresentation')

# get color transfer function/color map for 'velocity'
velocityLUT = GetColorTransferFunction('velocity')
velocityLUT.RGBPoints = [0.0, 0.231373, 0.298039, 0.752941, 9.822585922683267e-05, 0.865003, 0.865003, 0.865003, 0.00019645171845366534, 0.705882, 0.0156863, 0.14902]
velocityLUT.ScalarRangeInitialized = 1.0

# get opacity transfer function/opacity map for 'velocity'
velocityPWF = GetOpacityTransferFunction('velocity')
velocityPWF.Points = [0.0, 0.0, 0.5, 0.0, 0.00019645171845366534, 1.0, 0.5, 0.0]
velocityPWF.ScalarRangeInitialized = 1

# trace defaults for the display properties.
fluidpvdDisplay.Representation = 'Volume'
fluidpvdDisplay.ColorArrayName = ['POINTS', 'velocity']
fluidpvdDisplay.LookupTable = velocityLUT
fluidpvdDisplay.SelectTCoordArray = 'None'
fluidpvdDisplay.SelectNormalArray = 'None'
fluidpvdDisplay.SelectTangentArray = 'None'
fluidpvdDisplay.OSPRayScaleArray = 'velocity'
fluidpvdDisplay.OSPRayScaleFunction = 'PiecewiseFunction'
fluidpvdDisplay.SelectOrientationVectors = 'None'
fluidpvdDisplay.ScaleFactor = 3.9000000000000004
fluidpvdDisplay.SelectScaleArray = 'None'
fluidpvdDisplay.GlyphType = 'Arrow'
fluidpvdDisplay.GlyphTableIndexArray = 'None'
fluidpvdDisplay.GaussianRadius = 0.195
fluidpvdDisplay.SetScaleArray = ['POINTS', 'velocity']
fluidpvdDisplay.ScaleTransferFunction = 'PiecewiseFunction'
fluidpvdDisplay.OpacityArray = ['POINTS', 'velocity']
fluidpvdDisplay.OpacityTransferFunction = 'PiecewiseFunction'
fluidpvdDisplay.DataAxesGrid = 'GridAxesRepresentation'
fluidpvdDisplay.PolarAxes = 'PolarAxesRepresentation'
fluidpvdDisplay.ScalarOpacityUnitDistance = 1.9613602398106802
fluidpvdDisplay.ScalarOpacityFunction = velocityPWF
fluidpvdDisplay.OpacityArrayName = ['POINTS', 'velocity']
fluidpvdDisplay.SliceFunction = 'Plane'
fluidpvdDisplay.Slice = 19

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
fluidpvdDisplay.ScaleTransferFunction.Points = [-2.071833916422389e-06, 0.0, 0.5, 0.0, 2.071833916422389e-06, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
fluidpvdDisplay.OpacityTransferFunction.Points = [-2.071833916422389e-06, 0.0, 0.5, 0.0, 2.071833916422389e-06, 1.0, 0.5, 0.0]

# init the 'Plane' selected for 'SliceFunction'
fluidpvdDisplay.SliceFunction.Origin = [9.5, 9.5, 19.5]

# setup the color legend parameters for each legend in this view

# get color legend/bar for velocityLUT in view renderView1
velocityLUTColorBar = GetScalarBar(velocityLUT, renderView1)
velocityLUTColorBar.Title = 'velocity'
velocityLUTColorBar.ComponentTitle = 'Magnitude'

# set color bar visibility
velocityLUTColorBar.Visibility = 1

# show color legend
fluidpvdDisplay.SetScalarBarVisibility(renderView1, True)

# ----------------------------------------------------------------
# setup color maps and opacity mapes used in the visualization
# note: the Get..() functions create a new object, if needed
# ----------------------------------------------------------------

# ----------------------------------------------------------------
# setup extractors
# ----------------------------------------------------------------

# create extractor
jPG1 = CreateExtractor('JPG', renderView1, registrationName='JPG1')
# trace defaults for the extractor.
# init the 'JPG' selected for 'Writer'
jPG1.Writer.FileName = 'RenderView1_%.6ts%cm.jpg'
jPG1.Writer.ImageResolution = [1156, 796]
jPG1.Writer.Format = 'JPEG'

# ----------------------------------------------------------------
# restore active source
SetActiveSource(jPG1)
# ----------------------------------------------------------------

# ------------------------------------------------------------------------------
# Catalyst options
from paraview import catalyst
options = catalyst.Options()
options.GlobalTrigger = 'TimeStep'
options.CatalystLiveTrigger = 'TimeStep'

# ------------------------------------------------------------------------------
if __name__ == '__main__':
    from paraview.simple import SaveExtractsUsingCatalystOptions
    # Code for non in-situ environments; if executing in post-processing
    # i.e. non-Catalyst mode, let's generate extracts using Catalyst options
    SaveExtractsUsingCatalystOptions(options)
