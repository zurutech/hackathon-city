from dataclasses import dataclass, field

@dataclass
class Rotator:
    Pitch: float
    Yaw: float
    Roll: float

@dataclass
class Scale3D:
    X: float
    Y: float
    Z: float

@dataclass
class Option:
    BaseObject: str

@dataclass
class AdjacencyOption:
    direction: str
    options: list[Option] = field(default_factory=list)

@dataclass
class Constraint:
    BaseObject: str
    BaseRotator: Rotator
    BaseScale3D: Scale3D
    AdjacencyToOptionsMap: list[AdjacencyOption] = field(default_factory=list)

@dataclass
class ModelData:
    Constraints: list[Constraint] = field(default_factory=list)
