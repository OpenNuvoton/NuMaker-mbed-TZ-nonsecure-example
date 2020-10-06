# Example for non-PSA secure/non-secure code on Nuvoton TrustZone targets

This is an example to demonstrate non-PSA secure/non-secure code running on Nuvoton TrustZone targets.

- [Secure code](https://github.com/OpenNuvoton/NuMaker-mbed-TZ-secure-example)
- [Non-secure code](https://github.com/OpenNuvoton/NuMaker-mbed-TZ-nonsecure-example)

## Supported platforms

- NuMaker-PFM-M2351

In the following, we take **NuMaker-PFM-M2351** as an example for explanation.

## Supported toolchain

- ARM Compiler 6.10 or afterwards

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

On **NuMaker-PFM-M2351** board, it has 512 KiB flash in total. Secure/non-secure flash addresses start
at `0x00000000`/`0x10000000` respectively. In the following example, we would partition the flash
into 256 KiB/256 KiB for secure/non-secure worlds respectively.

- **Secure flash:** `0x00000000-0x0003FFFF`
- **Non-secure flash:** `0x10040000-0x1007FFFF`

### Partition the SRAM

On **NuMaker-PFM-M2351** board, it has 96 KiB SRAM in total. Secure/non-secure SRAM addresses start
at `0x20000000`/`0x30000000` respectively. In the following example, we would partition the SRAM
into 32 KiB/64 KiB for secure/non-secure worlds respectively.

- **Secure SRAM:** `0x20000000-0x20007FFF`
- **Non-secure SRAM:** `0x30008000-0x30017FFF`

### Partition the peripherals

TODO

## Compile TrustZone non-PSA secure code

Follow the steps below to compile TrustZone non-PSA secure code.

1.  Clone NuMaker-mbed-TZ-secure-example
    ```sh
    mbed import https://github.com/OpenNuvoton/NuMaker-mbed-TZ-secure-example
    cd NuMaker-mbed-TZ-secure-example
    ```

1.  Tell Mbed of secure flash/SRAM partition for secure world

    To compile TrustZone non-PSA secure code, we need to tell Mbed build system of secure flash/SRAM partition for secure world. The step is required.

    ```json
    {
        "target_overrides": {
            ......
            "NU_PFM_M2351_NPSA_S": {
                ......
                "target.mbed_rom_start"             : "0x0",
                "target.mbed_rom_size"              : "0x40000",
                "target.mbed_ram_start"             : "0x20000000",
                "target.mbed_ram_size"              : "0x8000",
                ......
            }
        }
    }
    ```

1.  Don't enable RTOS

    The [secure code](https://github.com/OpenNuvoton/NuMaker-mbed-TZ-nonsecure-example) runs with RTOS disabled.
    Secure code with RTOS is not supported yet.
    ```json
    {
        "requires": ["bare-metal", "rtos-api"],
        ......
    }
    ```

1.  Tell Mbed of boot stack size when RTOS is absent for secure world (optional, preferred)

    The default boot stack size when RTOS is absent may not meet your requirement. Change it explicitly.

    ```json
    {
        "target_overrides": {
            ......
            "NU_PFM_M2351_NPSA_S": {
                ......
                "target.boot-stack-size"            : "0x1000",
                ......
            }
        }
    }
    ```

1.  Enable serial and so printf (optional, not preferred)

    The [secure code](https://github.com/OpenNuvoton/NuMaker-mbed-TZ-nonsecure-example) runs with serial disabled by default.
    This is preferred configuration for secure code to decrease memory footprint.
    
    To run with serial enabled, add "SERIAL", "SERIAL_ASYNCH", "SERIAL_FC", and "STDIO_MESSAGES" in the configuration option `target.device_has_add`.

    ```json
    {
        "target_overrides": {
            ......
            "NU_PFM_M2351_NPSA_S": {
                ......
                "target.device_has_add": ["SERIAL", "SERIAL_ASYNCH", "SERIAL_FC", "STDIO_MESSAGES"],
                ......
            }
        }
    }
    ```

    Besides, disable STDIO buffered serial explicitly as well.
    STDIO buffered serial needs USB UART INT.
    By default, both USB UART and its INT are configured as non-secure.
    Even though secure code can control this UART via non-secure data access, there's no USB UART INT in secure side.
    So disable this function explicitly for safe.

    ```json
    {
        "target_overrides": {
            "*": {
                ......
                "platform.stdio-buffered-serial"    : false
            },
            "NU_PFM_M2351_NPSA_S": {
                "target.device_has_add": ["SERIAL", "SERIAL_ASYNCH", "SERIAL_FC", "STDIO_MESSAGES"],
                ......
            }
        }
    }
    ```

    To run with serial disabled, add "SERIAL", "SERIAL_ASYNCH", "SERIAL_FC", and "STDIO_MESSAGES" in the configuration option `target.device_has_remove`.

    ```json
    {
        "target_overrides": {
            ......
            "NU_PFM_M2351_NPSA_S": {
                ......
                "target.device_has_remove": ["SERIAL", "SERIAL_ASYNCH", "SERIAL_FC", "STDIO_MESSAGES"],
                ......
            }
        }
    }
    ```

1.  Change non-secure jump address (optional)

    This is done by configuring `tz-start-ns` in `mbed_app.json`.
    If not specified, it defaults to start of non-secure flash.
    To jump to non-secure address `0x10044000`, `mbed_app.json` would have:
   
    ```json
    {
        "target_overrides": {
            ......
            "NU_PFM_M2351_NPSA_S": {
                ......
                "tz-start-ns": "0x10044000",
                ......
            }
        }
    }
    ```

1.	Compile by running command:
    ```sh
    mbed compile -m NU_PFM_M2351_NPSA_S -t ARM --app-config mbed_app_custom.json
    ```

1.	Flash compiled secure code

    This step is not necessary anymore because secure code and non-secure code will merge together in updated non-secure code build process.

1.	Keep compiled secure code for non-secure code build process

    - `BUILD/NU_PFM_M2351_NPSA_S/ARM/NuMaker-mbed-TZ-secure-example.hex`
    - `BUILD/NU_PFM_M2351_NPSA_S/ARM/cmse-lib.o`

## Compile TrustZone non-PSA non-secure code

Follow the steps below to compile TrustZone non-PSA non-secure code.

1. Clone NuMaker-mbed-TZ-nonsecure-example
    ```sh
    mbed import https://github.com/OpenNuvoton/NuMaker-mbed-TZ-nonsecure-example
    cd NuMaker-mbed-TZ-nonsecure-example`
    ```

1.  Tell Mbed of non-secure flash/SRAM partition for non-secure world

    To compile TrustZone non-secure code, we need to tell Mbed build system of non-secure
    flash/SRAM partition for non-secure world. If we skip the step, default non-secure partition
    would be applied. Usually, the step is required if we compile our own secure code as above
    which would change the default partition.

    ```json
    "target_overrides": {
        ......
        "NU_PFM_M2351_NPSA_NS": {
            ......
            "target.mbed_rom_start"             : "0x10040000",
            "target.mbed_rom_size"              : "0x40000",
            "target.mbed_ram_start"             : "0x30008000",
            "target.mbed_ram_size"              : "0x10000",
            .......
        }
    }
    ```

1.  Exclude pre-built secure image/gateway library

    There has been a pre-built secure code in mbed-os directory tree.
    To link with our own one as above, we must exclude the pre-built one.

    ```json
    "target_overrides": {
        ......
        "NU_PFM_M2351_NPSA_NS": {
            "target.extra_labels_remove"        : ["NU_PREBUILD_SECURE"],
            ......
        }
    }
    ```

1.  Add compiled secure code above into non-secure code build process

    Create **TARGET_NU_PFM_M2351_NPSA_NS** directory and copy `NuMaker-mbed-TZ-secure-example.hex`/`cmse_lib.o` (just built above) there.
    
1.	Compile by running command:

    ```sh
    mbed compile -m NU_PFM_M2351_NPSA_NS -t ARM --app-config mbed_app_custom.json
    ```

1.  Flash compiled secure/non-secure code together

    Drag-n-drop `BUILD/NU_PFM_M2351_NPSA_NS/ARM/NuMaker-mbed-TZ-nonsecure-example.hex` onto **NuMaker_PFM_M2351** board to flash compiled secure/non-secure code together.

## Execution

If everything goes well, you would see console log (**115200/8-N-1**) like:

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
