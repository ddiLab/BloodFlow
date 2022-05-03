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
renderView1.ViewSize = [1605, 1156]
renderView1.AxesGrid = 'GridAxes3DActor'
renderView1.OrientationAxesInteractivity = 1
renderView1.OrientationAxesLabelColor = [0.0, 0.0, 0.0]
renderView1.OrientationAxesOutlineColor = [0.0, 0.0, 0.0]
renderView1.CenterOfRotation = [45.0, 45.0, 75.0]
renderView1.StereoType = 'Crystal Eyes'
renderView1.CameraPosition = [-175.11535157708664, 179.28275553154586, 281.35497333133696]
renderView1.CameraFocalPoint = [46.768914440359275, 40.57901784637141, 79.99453130451803]
renderView1.CameraViewUp = [0.2632556846609543, 0.9052549999829015, -0.3334813780402878]
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
layout1.SetSize(1605, 1156)

# ----------------------------------------------------------------
# restore active view
SetActiveView(renderView1)
# ----------------------------------------------------------------

# ----------------------------------------------------------------
# setup the data processing pipelines
# ----------------------------------------------------------------

# create a new 'PVD Reader'
fluidpvd = PVDReader(registrationName='fluid', FileName='C:\\Users\\Connor\\Documents\\post3\\fluid.pvd')
fluidpvd.CellArrays = ['vtkTestType']
fluidpvd.PointArrays = ['velocity', 'vorticity', 'velocityNorm']

# create a new 'PVD Reader'
cellspvd = PVDReader(registrationName='cells', FileName='C:\\Users\\Connor\\Documents\\post3\\cells.pvd')

# create a new 'Generate Surface Normals'
generateSurfaceNormals1 = GenerateSurfaceNormals(registrationName='GenerateSurfaceNormals1', Input=cellspvd)

# create a new 'XML Image Data Reader'
fluid_000005_00000 = XMLImageDataReader(registrationName='fluid_000005_00000*', FileName=['C:\\Users\\Connor\\Documents\\post3\\fluid_000005_000000.vti', 'C:\\Users\\Connor\\Documents\\post3\\fluid_000005_000001.vti', 'C:\\Users\\Connor\\Documents\\post3\\fluid_000005_000002.vti', 'C:\\Users\\Connor\\Documents\\post3\\fluid_000005_000003.vti', 'C:\\Users\\Connor\\Documents\\post3\\fluid_000005_000004.vti', 'C:\\Users\\Connor\\Documents\\post3\\fluid_000005_000005.vti', 'C:\\Users\\Connor\\Documents\\post3\\fluid_000005_000006.vti', 'C:\\Users\\Connor\\Documents\\post3\\fluid_000005_000007.vti', 'C:\\Users\\Connor\\Documents\\post3\\fluid_000005_000008.vti'])
fluid_000005_00000.CellArrayStatus = ['vtkTestType']
fluid_000005_00000.PointArrayStatus = ['velocity', 'vorticity', 'velocityNorm']
fluid_000005_00000.TimeArray = 'None'

# create a new 'Threshold'
threshold1 = Threshold(registrationName='Threshold1', Input=fluidpvd)
threshold1.Scalars = ['CELLS', 'vtkTestType']
threshold1.ThresholdRange = [0.0, 0.002951216551930837]

# create a new 'Resample To Image'
resampleToImage1 = ResampleToImage(registrationName='ResampleToImage1', Input=threshold1)
resampleToImage1.SamplingBounds = [0.0, 90.0, 0.0, 90.0, 0.0, 150.0]

# create a new 'Slice'
slice1 = Slice(registrationName='Slice1', Input=resampleToImage1)
slice1.SliceType = 'Plane'
slice1.HyperTreeGridSlicer = 'Plane'
slice1.SliceOffsetValues = [0.0]

# init the 'Plane' selected for 'SliceType'
slice1.SliceType.Origin = [45.0, 45.0, 5.0]
slice1.SliceType.Normal = [0.0, 0.0, 1.0]

# init the 'Plane' selected for 'HyperTreeGridSlicer'
slice1.HyperTreeGridSlicer.Origin = [45.0, 45.0, 75.0]

# ----------------------------------------------------------------
# setup the visualization in view 'renderView1'
# ----------------------------------------------------------------

# show data from resampleToImage1
resampleToImage1Display = Show(resampleToImage1, renderView1, 'UniformGridRepresentation')

# get color transfer function/color map for 'velocity'
velocityLUT = GetColorTransferFunction('velocity')
velocityLUT.RGBPoints = [0.0, 0.0, 0.0, 0.5625, 0.0003279126223015873, 0.0, 0.0, 1.0, 0.001077428663170634, 0.0, 1.0, 1.0, 0.0014521859458010193, 0.5, 1.0, 0.5, 0.0018269432284314048, 1.0, 1.0, 0.0, 0.0025764592693004514, 1.0, 0.0, 0.0, 0.002951216551930837, 0.5, 0.0, 0.0]
velocityLUT.ColorSpace = 'RGB'
velocityLUT.ScalarRangeInitialized = 1.0

# get opacity transfer function/opacity map for 'velocity'
velocityPWF = GetOpacityTransferFunction('velocity')
velocityPWF.Points = [0.0, 0.0, 0.5, 0.0, 0.002951216551930837, 1.0, 0.5, 0.0]
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
resampleToImage1Display.ScaleFactor = 14.999985000000004
resampleToImage1Display.SelectScaleArray = 'None'
resampleToImage1Display.GlyphType = 'Arrow'
resampleToImage1Display.GlyphTableIndexArray = 'None'
resampleToImage1Display.GaussianRadius = 0.7499992500000002
resampleToImage1Display.SetScaleArray = ['POINTS', 'velocity']
resampleToImage1Display.ScaleTransferFunction = 'PiecewiseFunction'
resampleToImage1Display.OpacityArray = ['POINTS', 'velocity']
resampleToImage1Display.OpacityTransferFunction = 'PiecewiseFunction'
resampleToImage1Display.DataAxesGrid = 'GridAxesRepresentation'
resampleToImage1Display.PolarAxes = 'PolarAxesRepresentation'
resampleToImage1Display.ScalarOpacityUnitDistance = 29.508443850885648
resampleToImage1Display.ScalarOpacityFunction = velocityPWF
resampleToImage1Display.OpacityArrayName = ['POINTS', 'velocity']
resampleToImage1Display.SliceFunction = 'Plane'
resampleToImage1Display.Slice = 49

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
resampleToImage1Display.OSPRayScaleFunction.Points = [7.579073974147171e-06, 0.0, 0.5, 0.0, 0.5382056629392598, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
resampleToImage1Display.ScaleTransferFunction.Points = [-6.245484490187e-06, 0.0, 0.5, 0.0, 6.245484490187043e-06, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
resampleToImage1Display.OpacityTransferFunction.Points = [-6.245484490187e-06, 0.0, 0.5, 0.0, 6.245484490187043e-06, 1.0, 0.5, 0.0]

# init the 'Plane' selected for 'SliceFunction'
resampleToImage1Display.SliceFunction.Origin = [45.0, 45.0, 75.0]

# show data from fluid_000005_00000
fluid_000005_00000Display = Show(fluid_000005_00000, renderView1, 'UniformGridRepresentation')

# trace defaults for the display properties.
fluid_000005_00000Display.Representation = 'Volume'
fluid_000005_00000Display.ColorArrayName = ['POINTS', 'velocity']
fluid_000005_00000Display.LookupTable = velocityLUT
fluid_000005_00000Display.SelectTCoordArray = 'None'
fluid_000005_00000Display.SelectNormalArray = 'None'
fluid_000005_00000Display.SelectTangentArray = 'None'
fluid_000005_00000Display.OSPRayScaleArray = 'velocity'
fluid_000005_00000Display.OSPRayScaleFunction = 'PiecewiseFunction'
fluid_000005_00000Display.SelectOrientationVectors = 'None'
fluid_000005_00000Display.ScaleFactor = 9.600000000000001
fluid_000005_00000Display.SelectScaleArray = 'None'
fluid_000005_00000Display.GlyphType = 'Arrow'
fluid_000005_00000Display.GlyphTableIndexArray = 'None'
fluid_000005_00000Display.GaussianRadius = 0.48
fluid_000005_00000Display.SetScaleArray = ['POINTS', 'velocity']
fluid_000005_00000Display.ScaleTransferFunction = 'PiecewiseFunction'
fluid_000005_00000Display.OpacityArray = ['POINTS', 'velocity']
fluid_000005_00000Display.OpacityTransferFunction = 'PiecewiseFunction'
fluid_000005_00000Display.DataAxesGrid = 'GridAxesRepresentation'
fluid_000005_00000Display.PolarAxes = 'PolarAxesRepresentation'
fluid_000005_00000Display.ScalarOpacityUnitDistance = 1.8926145047315737
fluid_000005_00000Display.ScalarOpacityFunction = velocityPWF
fluid_000005_00000Display.OpacityArrayName = ['POINTS', 'velocity']
fluid_000005_00000Display.SliceFunction = 'Plane'
fluid_000005_00000Display.Slice = 27

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
fluid_000005_00000Display.OSPRayScaleFunction.Points = [7.579073974147171e-06, 0.0, 0.5, 0.0, 0.5382056629392598, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
fluid_000005_00000Display.ScaleTransferFunction.Points = [-3.1279459887032784e-08, 0.0, 0.5, 0.0, 7.476422670916196e-06, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
fluid_000005_00000Display.OpacityTransferFunction.Points = [-3.1279459887032784e-08, 0.0, 0.5, 0.0, 7.476422670916196e-06, 1.0, 0.5, 0.0]

# init the 'Plane' selected for 'SliceFunction'
fluid_000005_00000Display.SliceFunction.Origin = [68.0, 45.0, 125.5]

# show data from slice1
slice1Display = Show(slice1, renderView1, 'GeometryRepresentation')

# trace defaults for the display properties.
slice1Display.Representation = 'Surface'
slice1Display.ColorArrayName = ['POINTS', 'velocity']
slice1Display.LookupTable = velocityLUT
slice1Display.SelectTCoordArray = 'None'
slice1Display.SelectNormalArray = 'None'
slice1Display.SelectTangentArray = 'None'
slice1Display.OSPRayScaleFunction = 'PiecewiseFunction'
slice1Display.SelectOrientationVectors = 'None'
slice1Display.ScaleFactor = -2.0000000000000002e+298
slice1Display.SelectScaleArray = 'None'
slice1Display.GlyphType = 'Arrow'
slice1Display.GlyphTableIndexArray = 'None'
slice1Display.GaussianRadius = -1e+297
slice1Display.SetScaleArray = [None, '']
slice1Display.ScaleTransferFunction = 'PiecewiseFunction'
slice1Display.OpacityArray = [None, '']
slice1Display.OpacityTransferFunction = 'PiecewiseFunction'
slice1Display.DataAxesGrid = 'GridAxesRepresentation'
slice1Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
slice1Display.OSPRayScaleFunction.Points = [7.579073974147171e-06, 0.0, 0.5, 0.0, 0.5382056629392598, 1.0, 0.5, 0.0]

# show data from generateSurfaceNormals1
generateSurfaceNormals1Display = Show(generateSurfaceNormals1, renderView1, 'GeometryRepresentation')

# get color transfer function/color map for 'vtkBlockColors'
vtkBlockColorsLUT = GetColorTransferFunction('vtkBlockColors')
vtkBlockColorsLUT.InterpretValuesAsCategories = 1
vtkBlockColorsLUT.AnnotationsInitialized = 1
vtkBlockColorsLUT.RGBPoints = [7.579073974147171e-06, 0.301961, 0.047059, 0.090196, 0.008828778147533358, 0.396078431372549, 0.0392156862745098, 0.058823529411764705, 0.017649977221092537, 0.49411764705882355, 0.054901960784313725, 0.03529411764705882, 0.026471176294651778, 0.5882352941176471, 0.11372549019607843, 0.023529411764705882, 0.035292375368210956, 0.6627450980392157, 0.16862745098039217, 0.01568627450980392, 0.04411357444177017, 0.7411764705882353, 0.22745098039215686, 0.00392156862745098, 0.05293477351532938, 0.788235294117647, 0.2901960784313726, 0.0, 0.061755972588888576, 0.8627450980392157, 0.3803921568627451, 0.011764705882352941, 0.07057717166244779, 0.9019607843137255, 0.4588235294117647, 0.027450980392156862, 0.07939837073600699, 0.9176470588235294, 0.5215686274509804, 0.047058823529411764, 0.0882195698095662, 0.9254901960784314, 0.5803921568627451, 0.0784313725490196, 0.09704076888312539, 0.9372549019607843, 0.6431372549019608, 0.12156862745098039, 0.10586196795668461, 0.9450980392156862, 0.7098039215686275, 0.1843137254901961, 0.11468316703024381, 0.9529411764705882, 0.7686274509803922, 0.24705882352941178, 0.12350436610380301, 0.9647058823529412, 0.8274509803921568, 0.3254901960784314, 0.13232556517736221, 0.9686274509803922, 0.8784313725490196, 0.4235294117647059, 0.1411467642509214, 0.9725490196078431, 0.9176470588235294, 0.5137254901960784, 0.14996796332448062, 0.9803921568627451, 0.9490196078431372, 0.596078431372549, 0.1587891623980398, 0.9803921568627451, 0.9725490196078431, 0.6705882352941176, 0.16761036147159902, 0.9882352941176471, 0.9882352941176471, 0.7568627450980392, 0.17607871258221586, 0.984313725490196, 0.9882352941176471, 0.8549019607843137, 0.17643156054515824, 0.9882352941176471, 0.9882352941176471, 0.8588235294117647, 0.17643694252599687, 0.9529411764705882, 0.9529411764705882, 0.8941176470588236, 0.17643694252599687, 0.9529411764705882, 0.9529411764705882, 0.8941176470588236, 0.18559749198663758, 0.8901960784313725, 0.8901960784313725, 0.807843137254902, 0.19475804144727826, 0.8274509803921568, 0.8235294117647058, 0.7372549019607844, 0.20391859090791895, 0.7764705882352941, 0.7647058823529411, 0.6784313725490196, 0.21307914036855963, 0.7254901960784313, 0.7137254901960784, 0.6274509803921569, 0.2222396898292003, 0.6784313725490196, 0.6627450980392157, 0.5803921568627451, 0.231400239289841, 0.6313725490196078, 0.6078431372549019, 0.5333333333333333, 0.24056078875048167, 0.5803921568627451, 0.5568627450980392, 0.48627450980392156, 0.24972133821112238, 0.5372549019607843, 0.5058823529411764, 0.44313725490196076, 0.25888188767176307, 0.4980392156862745, 0.4588235294117647, 0.40784313725490196, 0.2680424371324038, 0.4627450980392157, 0.4196078431372549, 0.37254901960784315, 0.27720298659304443, 0.43137254901960786, 0.38823529411764707, 0.34509803921568627, 0.28636353605368514, 0.403921568627451, 0.3568627450980392, 0.3176470588235294, 0.2955240855143258, 0.37254901960784315, 0.3215686274509804, 0.29411764705882354, 0.3046846349749665, 0.34509803921568627, 0.29411764705882354, 0.26666666666666666, 0.31384518443560716, 0.3176470588235294, 0.2627450980392157, 0.23921568627450981, 0.32300573389624787, 0.28627450980392155, 0.23137254901960785, 0.21176470588235294, 0.3321662833568886, 0.2549019607843137, 0.2, 0.1843137254901961, 0.34132683281752924, 0.23137254901960785, 0.17254901960784313, 0.16470588235294117, 0.35048738227816995, 0.2, 0.1450980392156863, 0.13725490196078433, 0.3596533137196493, 0.14902, 0.196078, 0.278431, 0.3775085486416103, 0.2, 0.2549019607843137, 0.34509803921568627, 0.3953637835635713, 0.24705882352941178, 0.3176470588235294, 0.41568627450980394, 0.4132190184855324, 0.3058823529411765, 0.38823529411764707, 0.49411764705882355, 0.43107425340749345, 0.37254901960784315, 0.4588235294117647, 0.5686274509803921, 0.4489294883294545, 0.44313725490196076, 0.5333333333333333, 0.6431372549019608, 0.4667847232514155, 0.5176470588235295, 0.615686274509804, 0.7254901960784313, 0.48463995817337663, 0.6, 0.6980392156862745, 0.8, 0.5024951930953376, 0.6862745098039216, 0.7843137254901961, 0.8705882352941177, 0.5203504280172987, 0.7607843137254902, 0.8588235294117647, 0.9294117647058824, 0.5292780454782793, 0.807843137254902, 0.9019607843137255, 0.9607843137254902, 0.5382056629392598, 0.8901960784313725, 0.9568627450980393, 0.984313725490196]
vtkBlockColorsLUT.ColorSpace = 'Lab'
vtkBlockColorsLUT.Annotations = ['0', '0', '1', '1', '2', '2', '3', '3', '4', '4', '5', '5', '6', '6', '7', '7', '8', '8', '9', '9', '10', '10', '11', '11']
vtkBlockColorsLUT.ActiveAnnotatedValues = ['0', '1', '2', '3', '4', '5']
vtkBlockColorsLUT.IndexedColors = [1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.63, 0.63, 1.0, 0.67, 0.5, 0.33, 1.0, 0.5, 0.75, 0.53, 0.35, 0.7, 1.0, 0.75, 0.5]

# trace defaults for the display properties.
generateSurfaceNormals1Display.Representation = 'Surface'
generateSurfaceNormals1Display.ColorArrayName = ['FIELD', 'vtkBlockColors']
generateSurfaceNormals1Display.LookupTable = vtkBlockColorsLUT
generateSurfaceNormals1Display.SelectTCoordArray = 'None'
generateSurfaceNormals1Display.SelectNormalArray = 'Normals'
generateSurfaceNormals1Display.SelectTangentArray = 'None'
generateSurfaceNormals1Display.OSPRayScaleArray = 'Normals'
generateSurfaceNormals1Display.OSPRayScaleFunction = 'PiecewiseFunction'
generateSurfaceNormals1Display.SelectOrientationVectors = 'None'
generateSurfaceNormals1Display.ScaleFactor = 15.3003
generateSurfaceNormals1Display.SelectScaleArray = 'None'
generateSurfaceNormals1Display.GlyphType = 'Arrow'
generateSurfaceNormals1Display.GlyphTableIndexArray = 'None'
generateSurfaceNormals1Display.GaussianRadius = 0.765015
generateSurfaceNormals1Display.SetScaleArray = ['POINTS', 'Normals']
generateSurfaceNormals1Display.ScaleTransferFunction = 'PiecewiseFunction'
generateSurfaceNormals1Display.OpacityArray = ['POINTS', 'Normals']
generateSurfaceNormals1Display.OpacityTransferFunction = 'PiecewiseFunction'
generateSurfaceNormals1Display.DataAxesGrid = 'GridAxesRepresentation'
generateSurfaceNormals1Display.PolarAxes = 'PolarAxesRepresentation'

# init the 'PiecewiseFunction' selected for 'OSPRayScaleFunction'
generateSurfaceNormals1Display.OSPRayScaleFunction.Points = [7.579073974147171e-06, 0.0, 0.5, 0.0, 0.5382056629392598, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
generateSurfaceNormals1Display.ScaleTransferFunction.Points = [-0.9999935030937195, 0.0, 0.5, 0.0, 0.9999852776527405, 1.0, 0.5, 0.0]

# init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
generateSurfaceNormals1Display.OpacityTransferFunction.Points = [-0.9999935030937195, 0.0, 0.5, 0.0, 0.9999852776527405, 1.0, 0.5, 0.0]

# ----------------------------------------------------------------
# setup color maps and opacity mapes used in the visualization
# note: the Get..() functions create a new object, if needed
# ----------------------------------------------------------------

# get opacity transfer function/opacity map for 'vtkBlockColors'
vtkBlockColorsPWF = GetOpacityTransferFunction('vtkBlockColors')
vtkBlockColorsPWF.Points = [7.579073974147171e-06, 0.0, 0.5, 0.0, 0.5382056629392598, 1.0, 0.5, 0.0]

# ----------------------------------------------------------------
# setup extractors
# ----------------------------------------------------------------

# create extractor
pNG1 = CreateExtractor('PNG', renderView1, registrationName='PNG1')
# trace defaults for the extractor.
# init the 'PNG' selected for 'Writer'
pNG1.Writer.FileName = 'RenderView1_%.6ts%cm.png'
pNG1.Writer.ImageResolution = [1605, 1156]
pNG1.Writer.Format = 'PNG'

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
options.CatalystLiveTrigger = 'TimeStep'

# ------------------------------------------------------------------------------
if __name__ == '__main__':
    from paraview.simple import SaveExtractsUsingCatalystOptions
    # Code for non in-situ environments; if executing in post-processing
    # i.e. non-Catalyst mode, let's generate extracts using Catalyst options
    SaveExtractsUsingCatalystOptions(options)
