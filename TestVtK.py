import sys
import vtk #vtk.util.numpy_support as vtknp
#import numpy as np
import glob
import math

nx = 10
ny = 10
nz = 10

imageData = vtk.vtkImageData

imageData.SetDimensions(nx, ny, nz)

director = vtk.vtkDoubleArray

director.SetNumberOfComponents(3)
director.SetNumberOfTuples(nx * ny * nz)

energy = vtk.vtkDoubleArray

energy.SetNumberOfComponents(1)
energy.SetNumberOfTuples(nx * ny * nz)

for i in range(director.SetNumberOfTuples):
    t = 1.0
    p = 0.0
    e = 5.0
    x = math.sin(t) * math.cos(p)
    y = math.sin(t) * math.sin(p)
    z = math.cos(t)
    director.SetTuple3(i, x, y, z)
    energy.SetValue(i, e)

imageData.GetPointData().AddArray(director)
director.SetName("Director")

imageData.GetPointData().AddArray(energy)
energy.SetName("Energy")

writer = vtk.vtkSmartPointer

writer.SetFileName("test.vti")

writer.SetInputData(imageData)

writer.Write()
