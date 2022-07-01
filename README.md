**Modmap detector**:
- Checks for extended modules, via looping through the peb->Ldr list, then checking each executable page if it's outside executable section
