# Example for secure/non-secure code on Nuvoton TrustZone targets

This is an example to demonstrate secure/non-secure code running on Nuvoton TrustZone targets.

- [Secure code](https://github.com/OpenNuvoton/NuMaker-mbed-TZ-secure-example)
- [Non-secure code](https://github.com/OpenNuvoton/NuMaker-mbed-TZ-nonsecure-example)

## Supported platforms

- NuMaker-PFM-M2351

In the following, we take **NUMAKER_PFM_M2351** as example target for explanation.

## Supported toolchain

- ARM Compiler 6.10

In the following, we compile our program with **ARMC6** toolchain.

## Partition the hardware

On TrustZone targets, it is necessary to partition the hardware first for secure/non-secure code
to run on secure/non-secure worlds respectively.

On Nuvoton's TrustZone chips, it is necessary to **Mass Erase** the chip first by external tool
e.g. **ICP Tool** before we can partition the hardware (only once). Fortunately, if we flash
secure code through drag-n-drop, Nu-Link would issue **Mass Erase** in its erase/program flow.
With that, secure code can get one chance to re-partition the hardware without resorting to
other tools.

### Partition the flash

On **NUMAKER_PFM_M2351**, it has 512 KiB flash in total. Secure/non-secure flash addresses start
at `0x00000000`/`0x10000000` respectively. In the following example, we would partition the flash
into 256 KiB/256 KiB for secure/non-secure worlds respectively.

**Secure flash:** `0x00000000-0x0003FFFF`

**Non-secure flash:** `0x10040000-0x1007FFFF`

### Partition the SRAM

On **NUMAKER_PFM_M2351**, it has 96 KiB SRAM in total. Secure/non-secure SRAM addresses start
at `0x20000000`/`0x30000000` respectively. In the following example, we would partition the SRAM
into 32 KiB/64 KiB for secure/non-secure worlds respectively.

**Secure SRAM:** `0x20000000-0x20007FFF`

**Non-secure SRAM:** `0x30008000-0x30017FFF`

### Partition the peripherals

TODO

## Compile TrustZone secure code

Follow the steps below to compile TrustZone secure code.

1.  Clone NuMaker-mbed-TZ-secure-example

    `mbed import https://github.com/OpenNuvoton/NuMaker-mbed-TZ-secure-example`

    `cd NuMaker-mbed-TZ-secure-example`

1.  Tell Mbed of secure flash/SRAM partition for secure world

    To compile TrustZone secure code, we need to tell Mbed build system of secure flash/SRAM
    partition for secure world. The step is required.

    <pre>
    {
        "target_overrides": {
            "NUMAKER_PFM_M2351": {
                "target.core": "Cortex-M23",
                "target.inherits": ["NUMAKER_PFM_M2351"],
                "target.device_has_remove": ["TRNG", "SERIAL", "SERIAL_ASYNCH", "SERIAL_FC", "STDIO_MESSAGES"],
                <b>
                "target.mbed_rom_start":    "0x0",
                "target.mbed_rom_size":     "0x40000",
                "target.mbed_ram_start":    "0x20000000",
                "target.mbed_ram_size":     "0x8000"
                </b>
            }
        }
    }
    </pre>

1.  Enable RTOS (optional, not preferred)

    The [secure code](https://github.com/OpenNuvoton/NuMaker-mbed-TZ-nonsecure-example) runs with RTOS disabled by default.
    This is preferred configuration for secure code to decrease memory footprint.
    To run with RTOS enabled, clear up all the content in `.mbedignore`.

1.  Enable serial and so printf (optional, not preferred)

    The [secure code](https://github.com/OpenNuvoton/NuMaker-mbed-TZ-nonsecure-example) runs with serial disabled by default.
    This is preferred configuration for secure code to decrease memory footprint.
    To run with serial enabled, remove "SERIAL" and the like in the configuration option `target.device_has_remove`.
   
1.  Change non-secure jump address (optional)

    This is done by configuring `tz-start-ns` in `mbed_app.json`.
    If not specified, it defaults to start of non-secure flash.
    To jump to non-secure address `0x10044000`, `mbed_app.json` would have:
   
    <pre>
    {
        "config": {
            "tz-start-ns": {
                "help": "Start address of TrustZone non-secure application"
            }
        },
        "target_overrides": {
            "*": {
                "platform.stdio-baud-rate": 9600,
                "platform.stdio-convert-newlines": true
            },
            "NUMAKER_PFM_M2351": {
                <b>"tz-start-ns": "0x10044000",</b>
                "target.core": "Cortex-M23",
                "target.inherits": ["NUMAKER_PFM_M2351"],
                "target.device_has_remove": ["TRNG", "SERIAL", "SERIAL_ASYNCH", "SERIAL_FC", "STDIO_MESSAGES"],
                
                "target.mbed_rom_start":    "0x0",
                "target.mbed_rom_size":     "0x40000",
                "target.mbed_ram_start":    "0x20000000",
                "target.mbed_ram_size":     "0x8000"
            }
        }
    }
    </pre>


1.	Compile by running command:

    `mbed compile -m NUMAKER_PFM_M2351 -t ARMC6`
    
1.	Flash compiled secure code

    Drag-n-drop `BUILD/NUMAKER_PFM_M2351/ARMC6/NuMaker-mbed-TZ-secure-example.hex` onto **NUMAKER_PFM_M2351** board to flash compiled secure code.

1.	Keep secure gateway library

    Keep `BUILD/NUMAKER_PFM_M2351/ARMC6/cmse-lib.o` for later use.


## Compile TrustZone non-secure code

Follow the steps below to compile TrustZone non-secure code.

1. Clone NuMaker-mbed-TZ-nonsecure-example

    `mbed import https://github.com/OpenNuvoton/NuMaker-mbed-TZ-nonsecure-example`

    `cd NuMaker-mbed-TZ-nonsecure-example`

1.  Tell Mbed of non-secure flash/SRAM partition for non-secure world

    To compile TrustZone non-secure code, we need to tell Mbed build system of non-secure
    flash/SRAM partition for non-secure world. If we skip the step, default non-secure partition
    would be applied. Usually, the step is required if we compile our own secure code as above
    which would change the default partition.

    <pre>
    {
        "target_overrides": {
            "*": {
                "platform.stdio-baud-rate": 9600,
                "platform.stdio-convert-newlines": true
            },
            "NUMAKER_PFM_M2351": {
                <b>
                "target.mbed_rom_start":    "0x10040000",
                "target.mbed_rom_size":     "0x40000",
                "target.mbed_ram_start":    "0x30008000",
                "target.mbed_ram_size":     "0x10000"
                </b>
            }
        }
    }
    </pre>

1.  Copy secure gateway library

    Create TARGET_NUMAKER_PFM_M2351 directory and copy `cmse_lib.o` (just built above) here.
    
1.	Compile by running command:

    `mbed compile -m NUMAKER_PFM_M2351 -t ARMC6`

1.  Flash compiled non-secure code

    Drag-n-drop `BUILD/NUMAKER_PFM_M2351/ARMC6/NuMaker-mbed-TZ-nonsecure-example.hex` onto **NUMAKER_PFM_M2351** board to flash compiled non-secure code.

## Execution

If everything goes well, you would see console log (**9600/8-N-1**) like:

```
+---------------------------------------------+
|    Non-secure code is running ...           |
+---------------------------------------------+
Non-secure main thread: 0 
Non-secure main thread: 1 
Non-secure main thread: 2 
Non-secure main thread: 3 
Non-secure main thread: 4 
```

You wouldn't see console log emitted by secure code because serial is disabled by default on it.
