#!/usr/bin/env python

import cadquery as cq

sketch1 = cq.Workplane("XZ", origin=(0, 3, 0)).moveTo(0, 0).lineTo(50, 0).lineTo(45, 10).lineTo(5, 10).lineTo(0, 0).close()
extrernalTrapezoid = sketch1.extrude(41)
sketch2 = cq.Workplane("XZ").moveTo(4, 0).lineTo(46, 0).lineTo(41, 10).lineTo(9, 10).lineTo(4, 0).close()
internalTrapezoid = sketch2.extrude(36)
trapezoid = extrernalTrapezoid - internalTrapezoid
result = trapezoid

show_object(result)
cq.exporters.export(result, 'basisCharger.step')
cq.exporters.export(result, 'basisCharger.stl')