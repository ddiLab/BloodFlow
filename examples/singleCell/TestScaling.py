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
renderView1.ViewSize = [1715, 1156]
renderView1.AxesGrid = 'GridAxes3DActor'
renderView1.OrientationAxesLabelColor = [0.0, 0.0, 0.0]
renderView1.CenterOfRotation = [45.0, 45.0, 75.0]
renderView1.StereoType = 'Crystal Eyes'
renderView1.CameraPosition = [-198.40329247079194, 152.9826984124396, 270.2738340707172]
renderView1.CameraFocalPoint = [47.89533274902393, 47.44011813073467, 77.36159254763548]
renderView1.CameraViewUp = [0.20491441710582203, 0.9448901552232146, -0.2553285652339561]
renderView1.CameraFocalDisk = 1.0
renderView1.CameraParallelScale = 103.40212763768452
renderView1.Background = [1.0, 1.0, 1.0]
renderView1.BackEnd = 'OSPRay raycaster'
renderView1.OSPRayMaterialLibrary = materialLibrary1

SetActiveView(None)

# ----------------------------------------------------------------
# setup view layouts
# ----------------------------------------------------------------

# create new layout object 'Layout #1'
layout1 = CreateLayout(name='Layout #1')
layout1.AssignView(0, renderView1)
layout1.SetSize(1715, 1156)

# ----------------------------------------------------------------
# restore active view
SetActiveView(renderView1)
# ----------------------------------------------------------------

# ----------------------------------------------------------------
# setup the data processing pipelines
# ----------------------------------------------------------------

# create a new 'PVD Reader'
fluidpvd = PVDReader(registrationName='fluid', FileName='/home/murphyc/insituBloodFlow/BloodFlow/examples/singleCell/vtk_output/fluid.pvd')
fluidpvd.CellArrays = ['vtkTestType']
fluidpvd.PointArrays = ['velocity', 'vorticity', 'velocityNorm']

# create a new 'PVD Reader'
cellspvd = PVDReader(registrationName='cells', FileName='/home/murphyc/insituBloodFlow/BloodFlow/examples/singleCell/vtk_output/cells.pvd')

# create a new 'Threshold'
threshold1 = Threshold(registrationName='Threshold1', Input=fluidpvd)
threshold1.Scalars = ['CELLS', 'vtkTestType']
threshold1.ThresholdRange = [0.0, 0.002954547442779923]

# create a new 'Resample To Image'
resampleToImage1 = ResampleToImage(registrationName='ResampleToImage1', Input=threshold1)
resampleToImage1.SamplingBounds = [0.0, 90.0, 0.0, 90.0, 0.0, 150.0]

# create a new 'Slice'
slice1 = Slice(registrationName='Slice1', Input=resampleToImage1)
slice1.SliceType = 'Plane'
slice1.HyperTreeGridSlicer = 'Plane'
slice1.SliceOffsetValues = [0.0]

# init the 'Plane' selected for 'SliceType'
slice1.SliceType.Origin = [45.0, 45.0, 1.0]
slice1.SliceType.Normal = [0.0, 0.0, 1.0]

# init the 'Plane' selected for 'HyperTreeGridSlicer'
slice1.HyperTreeGridSlicer.Origin = [45.0, 45.0, 75.0]

# create a new 'Generate Surface Normals'
generateSurfaceNormals1 = GenerateSurfaceNormals(registrationName='GenerateSurfaceNormals1', Input=cellspvd)

# ----------------------------------------------------------------
# setup the visualization in view 'renderView1'
# ----------------------------------------------------------------

# show data from resampleToImage1
resampleToImage1Display = Show(resampleToImage1, renderView1, 'UniformGridRepresentation')

# get color transfer function/color map for 'velocity'
velocityLUT = GetColorTransferFunction('velocity')
velocityLUT.RGBPoints = [0.0, 0.0, 0.0, 0.5625, 0.00032811770494012765, 0.0, 0.0, 1.0, 0.0010781025070487108, 0.0, 1.0, 1.0, 0.001453094169837428, 0.5, 1.0, 0.5, 0.0018280858326261452, 1.0, 1.0, 0.0, 0.0025780706347347288, 1.0, 0.0, 0.0, 0.0029530622975234458, 0.5, 0.0, 0.0]
velocityLUT.ColorSpace = 'RGB'
velocityLUT.ScalarRangeInitialized = 1.0

# get opacity transfer function/opacity map for 'velocity'
velocityPWF = GetOpacityTransferFunction('velocity')
velocityPWF.Points = [0.0, 0.0, 0.5, 0.0, 0.0029530622975234458, 1.0, 0.5, 0.0]
velocityPWF.ScalarRangeInitialized = 1

# trace defaults for the display properties.
resampleToImage1Display.Representation = 'Volume'
resampleToImage1Display.ColorArrayName = ['POINTS', 'velocity']
resampleToImage1Display.LookupTable = velocityLUT
resampleToImage1Display.SelectTCoordArray = 'None'
resampleToImage1Display.SelectNormalArray = 'None'
resampleToImage1Display.SelectTangentArray = 'None'
resampleToImage1Display.OSPRayScaleArray = 'velocity'
resampleToImage1Display.OSPRayScaleFunction = 'PiecewiseFunction'
resampleToImage1Display.SelectOrientationVectors = 'None'
resampleToImage1Display.ScaleFactor = 15.0
resampleToImage1Display.SelectScaleArray = 'None'
resampleToImage1Display.GlyphType = 'Arrow'
resampleToImage1Display.GlyphTableIndexArray = 'None'
resampleToImage1Display.GaussianRadius = 0.75
resampleToImage1Display.SetScaleArray = ['POINTS', 'velocity']
resampleToImage1Display.ScaleTransferFunction = 'PiecewiseFunction'
resampleToImage1Display.OpacityArray = ['POINTS', 'velocity']
resampleToImage1Display.OpacityTransferFunction = 'PiecewiseFunction'
resampleToImage1Display.DataAxesGrid = 'GridAxesRepresentation'
resampleToImage1Display.PolarAxes = 'PolarAxesRepresentation'
resampleToImage1Display.ScalarOpacityUnitDistance = 13.770620901034203
resampleToImage1Display.ScalarOpacityFunction = velocityPWF
resampleToImage1Display.OpacityArrayName = ['POINTS', 'velocity']
resampleToImage1Display.SliceFunction = 'Plane'
resampleToImage1Display.Slice = 49

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
resampleToImage1Display.OSPRayScaleFunction.Points = [7.579073974147171e-06, 0.0, 0.5, 0.0, 0.5382056629392598, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
resampleToImage1Display.ScaleTransferFunction.Points = [-1.3894410305812546e-05, 0.0, 0.5, 0.0, 1.757177823410226e-05, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
resampleToImage1Display.OpacityTransferFunction.Points = [-1.3894410305812546e-05, 0.0, 0.5, 0.0, 1.757177823410226e-05, 1.0, 0.5, 0.0]

# init the 'Plane' selected for 'SliceFunction'
resampleToImage1Display.SliceFunction.Origin = [45.0, 45.0, 75.0]

# show data from generateSurfaceNormals1
generateSurfaceNormals1Display = Show(generateSurfaceNormals1, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
generateSurfaceNormals1Display.Representation = 'Surface'
generateSurfaceNormals1Display.AmbientColor = [0.6666666666666666, 0.0, 0.0]
generateSurfaceNormals1Display.ColorArrayName = ['POINTS', '']
generateSurfaceNormals1Display.DiffuseColor = [0.6666666666666666, 0.0, 0.0]
generateSurfaceNormals1Display.SelectTCoordArray = 'None'
generateSurfaceNormals1Display.SelectNormalArray = 'Normals'
generateSurfaceNormals1Display.SelectTangentArray = 'None'
generateSurfaceNormals1Display.OSPRayScaleArray = 'Normals'
generateSurfaceNormals1Display.OSPRayScaleFunction = 'PiecewiseFunction'
generateSurfaceNormals1Display.SelectOrientationVectors = 'None'
generateSurfaceNormals1Display.ScaleFactor = 15.306374785095887
generateSurfaceNormals1Display.SelectScaleArray = 'None'
generateSurfaceNormals1Display.GlyphType = 'Arrow'
generateSurfaceNormals1Display.GlyphTableIndexArray = 'None'
generateSurfaceNormals1Display.GaussianRadius = 0.7653187392547943
generateSurfaceNormals1Display.SetScaleArray = ['POINTS', 'Normals']
generateSurfaceNormals1Display.ScaleTransferFunction = 'PiecewiseFunction'
generateSurfaceNormals1Display.OpacityArray = ['POINTS', 'Normals']
generateSurfaceNormals1Display.OpacityTransferFunction = 'PiecewiseFunction'
generateSurfaceNormals1Display.DataAxesGrid = 'GridAxesRepresentation'
generateSurfaceNormals1Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
generateSurfaceNormals1Display.OSPRayScaleFunction.Points = [7.579073974147171e-06, 0.0, 0.5, 0.0, 0.5382056629392598, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
generateSurfaceNormals1Display.ScaleTransferFunction.Points = [-0.9999932050704956, 0.0, 0.5, 0.0, 0.9999850988388062, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
generateSurfaceNormals1Display.OpacityTransferFunction.Points = [-0.9999932050704956, 0.0, 0.5, 0.0, 0.9999850988388062, 1.0, 0.5, 0.0]

# show data from slice1
slice1Display = Show(slice1, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
slice1Display.Representation = 'Surface'
slice1Display.ColorArrayName = ['POINTS', 'velocity']
slice1Display.LookupTable = velocityLUT
slice1Display.SelectTCoordArray = 'None'
slice1Display.SelectNormalArray = 'None'
slice1Display.SelectTangentArray = 'None'
slice1Display.OSPRayScaleArray = 'velocity'
slice1Display.OSPRayScaleFunction = 'PiecewiseFunction'
slice1Display.SelectOrientationVectors = 'None'
slice1Display.ScaleFactor = 9.0
slice1Display.SelectScaleArray = 'None'
slice1Display.GlyphType = 'Arrow'
slice1Display.GlyphTableIndexArray = 'None'
slice1Display.GaussianRadius = 0.45
slice1Display.SetScaleArray = ['POINTS', 'velocity']
slice1Display.ScaleTransferFunction = 'PiecewiseFunction'
slice1Display.OpacityArray = ['POINTS', 'velocity']
slice1Display.OpacityTransferFunction = 'PiecewiseFunction'
slice1Display.DataAxesGrid = 'GridAxesRepresentation'
slice1Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
slice1Display.OSPRayScaleFunction.Points = [7.579073974147171e-06, 0.0, 0.5, 0.0, 0.5382056629392598, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
slice1Display.ScaleTransferFunction.Points = [-7.97774208577706e-06, 0.0, 0.5, 0.0, 5.7684469975770325e-06, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
slice1Display.OpacityTransferFunction.Points = [-7.97774208577706e-06, 0.0, 0.5, 0.0, 5.7684469975770325e-06, 1.0, 0.5, 0.0]

# setup the color legend parameters for each legend in this view

# get color legend/bar for velocityLUT in view renderView1
velocityLUTColorBar = GetScalarBar(velocityLUT, renderView1)
velocityLUTColorBar.Orientation = 'Horizontal'
velocityLUTColorBar.WindowLocation = 'AnyLocation'
velocityLUTColorBar.Position = [0.30425655976676386, 0.9199653979238755]
velocityLUTColorBar.Title = 'Velocity (mm/s)'
velocityLUTColorBar.ComponentTitle = 'Magnitude'
velocityLUTColorBar.TitleColor = [0.0, 0.0, 0.0]
velocityLUTColorBar.LabelColor = [0.0, 0.0, 0.0]

# set color bar visibility
velocityLUTColorBar.Visibility = 1

# show color legend
resampleToImage1Display.SetScalarBarVisibility(renderView1, True)

# show color legend
slice1Display.SetScalarBarVisibility(renderView1, True)

# ----------------------------------------------------------------
# setup color maps and opacity mapes used in the visualization
# note: the Get..() functions create a new object, if needed
# ----------------------------------------------------------------

# ----------------------------------------------------------------
# setup extractors
# ----------------------------------------------------------------

# create extractor
#pNG1 = CreateExtractor('PNG', renderView1, registrationName='PNG1')
# trace defaults for the extractor.
# init the 'PNG' selected for 'Writer'
#pNG1.Writer.FileName = 'RenderView1_%.6ts%cm.png'
#pNG1.Writer.ImageResolution = [1715, 1156]
#pNG1.Writer.Format = 'PNG'

# ----------------------------------------------------------------
# restore active source
SetActiveSource(pNG1)
# ----------------------------------------------------------------

# ------------------------------------------------------------------------------
# Catalyst options
from paraview import catalyst
options = catalyst.Options()
options.GenerateCinemaSpecification = 1
options.GlobalTrigger = 'TimeStep'
options.EnableCatalystLive = 1
options.CatalystLiveURL = '10.156.208.148:11111'
options.CatalystLiveTrigger = 'TimeStep'

# ------------------------------------------------------------------------------
if __name__ == '__main__':
    from paraview.simple import SaveExtractsUsingCatalystOptions
    # Code for non in-situ environments; if executing in post-processing
    # i.e. non-Catalyst mode, let's generate extracts using Catalyst options
    SaveExtractsUsingCatalystOptions(options)
