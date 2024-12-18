from dataclasses import dataclass, field

@dataclass
class Rotator:
    Pitch: float = 0.0
    Yaw: float = 0.0
    Roll: float = 0.0

@dataclass
class Scale3D:
    X: float = 1.0
    Y: float = 1.0
    Z: float = 1.0

@dataclass
class Option:
    BaseObject: str
    BaseRotator: Rotator = field(default_factory=Rotator)
    BaseScale3D: Scale3D = field(default_factory=Scale3D)

@dataclass
class AdjacencyOption:
    direction: str
    options: list[Option] = field(default_factory=list)

@dataclass
class Constraint:
    BaseObject: str
    BaseRotator: Rotator = field(default_factory=Rotator)
    BaseScale3D: Scale3D = field(default_factory=Scale3D)
    Weight: float = 1.0
    AdjacencyToOptionsMap: list[AdjacencyOption] = field(default_factory=list)

@dataclass
class ModelData:
    Constraints: list[Constraint] = field(default_factory=list)
