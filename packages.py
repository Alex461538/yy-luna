import json
import dataclasses
from typing import List, Dict, Any

@dataclasses.dataclass
class Package:
    name: str = ""
    version: str = ""