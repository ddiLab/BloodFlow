import sys
import vtk
import glob
import math
from random import random, randint
nx = 10
ny = 10
nz = 10

ImageData = vtk.vtkImageData()
ImageData.SetDimensions(nx, ny, nz)

director = vtk.vtkDoubleArray()
director.SetNumberOfComponents(3)
director.SetNumberOfTuples(nx * ny * nz)

energy = vtk.vtkDoubleArray()
energy.SetNumberOfComponents(1)
energy.SetNumberOfTuples(nx * ny * nz)

Velocity_values = vtk.vtkDoubleArray()
Velocity_values.SetNumberOfComponents(3)
Velocity_values.SetNumberOfTuples(nx * ny * nz)

Velocity_Mag = vtk.vtkDoubleArray()
Velocity_Mag.SetNumberOfComponents(1)
Velocity_Mag.SetNumberOfTuples(nx * ny * nz)

for i in range(0, director.GetNumberOfTuples()):
    t = 1
    p = 0.0
    e = 5.0
    x = math.sin(t) * math.cos(p)
    y = math.sin(t) * math.sin(p)
    z = math.cos(t)

    director.SetTuple3(i, x, y, z)
    energy.SetValue(i, e)

#for i in range(1000):
    #vx = random()
    #vy = random()
    #vz = random()
    #Velocity_values = [vx, vy, vz]
   # print(Velocity_values)

for n in range(0, Velocity_values.GetNumberOfTuples()):
    vx = randint(-5, 5)
    vy = randint(-5, 5)
    vz = randint(-5, 5)
    mag = math.sqrt((vx**2)+(vy**2)+(vz**2))
    Velocity_values.SetTuple3(n, vx, vy, vz)
    Velocity_Mag.SetTuple1(n, mag)

ImageData.GetPointData().AddArray(director)
director.SetName("Director")

ImageData.GetPointData().AddArray(energy)
energy.SetName("Energy")

ImageData.GetPointData().AddArray(Velocity_values)
Velocity_values.SetName("Velocity")

ImageData.GetPointData().AddArray(Velocity_Mag)
Velocity_Mag.SetName("Velocity Mag.")

Writer = vtk.vtkXMLImageDataWriter()

Writer.SetInputData(ImageData)
Writer.SetFileName("Test.vti")
Writer.Write()
