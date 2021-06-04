# Example for non-PSA secure/non-secure code on Nuvoton TrustZone targets

This is an example to demonstrate non-PSA secure/non-secure code running on Nuvoton TrustZone targets.

- [Secure code](https://github.com/OpenNuvoton/NuMaker-mbed-TZ-secure-example)
- [Non-secure code](https://github.com/OpenNuvoton/NuMaker-mbed-TZ-nonsecure-example)

## Supported platforms

- NuMaker-PFM-M2351
- NuMaker-M2354

In the following, we take **NuMaker-PFM-M2351**/**NuMaker-M2354** as examples for explanation.

## Supported toolchain

- ARM Compiler 6.10 or afterwards

In the following, we compile our program with **ARMCLANG** toolchain.

## Partition the hardware

On TrustZone targets, it is necessary to partition the hardware first for secure/non-secure code
to run on secure/non-secure worlds respectively.

On Nuvoton's TrustZone chips, it is necessary to **Mass Erase** the chip first by external tool
e.g. **ICP Tool** before we can partition the hardware (only once). Fortunately, if we flash
secure code through drag-n-drop, Nu-Link/Nu-Link2 would issue **Mass Erase** in its erase/program flow.
With that, secure code can get one chance to re-partition the hardware without resorting to
other tools.

### Partition the flash

In the following example, we would partition the flash into secure/non-secure worlds,
where flash addresses start at `0x00000000`/(`0x10000000` + **Secure Flash Size**) respectively:

-   **M2351**:
    -   Total: 512 KiB
    -   Secure flash: 256 KiB (`0x00000000-0x0003FFFF`)
    -   Non-secure flash: 256 KiB (`0x10040000-0x1007FFFF`)
-   **M2354**:
    -   Total: 1024KiB
    -   Secure flash: 512 KiB (`0x00000000-0x0007FFFF`)
    -   Non-secure flash: 512 KiB (`0x10080000-0x100FFFFF`)

### Partition the SRAM

In the following example, we would partition the SRAM into secure/non-secure worlds,
where SRAM addresses start at `0x20000000`/(`0x30000000` + **Secure SRAM Size**) respectively:

-   **M2351**:
    -   Total: 96 KiB
    -   Secure SRAM: 32 KiB (`0x20000000-0x20007FFF`)
    -   Non-secure SRAM: 64 KiB (`0x30008000-0x30017FFF`)
-   **M2354**:
    -   Total: 256 KiB
    -   Secure SRAM: 128 KiB (`0x20000000-0x2001FFFF`)
    -   Non-secure SRAM: 128 KiB (`0x30020000-0x3003FFFF`)

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

    -   **M2351**:
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
    -   **M2354**:
    ```json
    {
        "target_overrides": {
            ......
            "NU_M2354_NPSA_S": {
                ......
                "target.mbed_rom_start"             : "0x0",
                "target.mbed_rom_size"              : "0x80000",
                "target.mbed_ram_start"             : "0x20000000",
                "target.mbed_ram_size"              : "0x20000"
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

1.  Adjust TZ process max stack slots/stack size
    ```json
    "macros": [
        "MBED_CONF_TZ_PROCESS_STACK_SLOTS=8",
        "MBED_CONF_TZ_PROCESS_STACK_SIZE=2048",
        ......
    ],
    ```

    **NOTE1**: TZ max stack slots `MBED_CONF_TZ_PROCESS_STACK_SLOTS` limits max NS threads which enable TZ access `MBED_TZ_DEFAULT_ACCESS`.

    **NOTE2**: TZ stack size `MBED_CONF_TZ_PROCESS_STACK_SIZE` is for secure library (NSC function). Its value is enlarged for custom NSC function.

    **NOTE3**: Avoid `printf`-like call in NSC function because it needs large stack and runs the risk of stack overflow.

1.  Tell Mbed of boot stack size when RTOS is absent for secure world (optional, preferred)

    The default boot stack size when RTOS is absent may not meet your requirement. Change it explicitly.

    -   **M2351**:
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
    -   **M2354**: Default boot stack size is fine, so no change to it.

1.  Enable serial and so printf (optional, not preferred)

    To run with serial enabled, add "SERIAL", "SERIAL_ASYNCH", "SERIAL_FC", and "STDIO_MESSAGES" in the configuration option `target.device_has_add`.

    -   **M2351**:
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
    -   **M2351**: Serial defaults to enabled.

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
                "platform.stdio-buffered-serial"    : false,
                ......
            },
            ......
        }
    }
    ```

    To run with serial disabled, add "SERIAL", "SERIAL_ASYNCH", "SERIAL_FC", and "STDIO_MESSAGES" in the configuration option `target.device_has_remove`.

    -   **M2351**:
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
    -   **M2354**:
    ```json
    {
        "target_overrides": {
            ......
            "NU_M2354_NPSA_S": {
                ......
                "target.device_has_remove": ["SERIAL", "SERIAL_ASYNCH", "SERIAL_FC", "STDIO_MESSAGES"],
                ......
            }
        }
    }
    ```

1.  Enable fault handler dump message (optional)
    ```json
    {
        "target_overrides": {
            ......
            "NU_PFM_M2351_NPSA_S": {
                ......
                "target.macros_remove": ["MBED_FAULT_HANDLER_DISABLED"],
                ......
            },
            "NU_M2354_NPSA_S": {
                ......
                "target.macros_remove": ["MBED_FAULT_HANDLER_DISABLED"],
                ......
            }
        }
    }
    ```
    **NOTE**: This gets unnecessary when fault handler dump message gets to enabled by default in future Mbed OS version.

1.  Change non-secure jump address (optional)

    This is done by configuring `tz-start-ns` in `mbed_app.json` or custom one.
    If not specified, it defaults to start of non-secure flash.
    To jump to non-default non-secure address like:

    -   **M2351**: `0x10044000`
    -   **M2354**: `0x10084000`
 
    `mbed_app.json` or custom one would have:

    -   **M2351**:
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
    -   **M2354**:
    ```json
    {
        "target_overrides": {
            ......
            "NU_M2354_NPSA_S": {
                ......
                "tz-start-ns": "0x10084000",
                ......
            }
        }
    }
    ```

1.	Compile by running command:

    -   **M2351**:
    ```sh
    mbed compile -m NU_PFM_M2351_NPSA_S -t ARM --app-config mbed_app_custom.json
    ```
    -   **M2354**:
    ```sh
    mbed compile -m NU_M2354_NPSA_S -t ARM --app-config mbed_app_custom.json
    ```

1.	Keep compiled secure code for non-secure code build process later

    -   **M2351**:
        -   `BUILD/NU_PFM_M2351_NPSA_S/ARM/NuMaker-mbed-TZ-secure-example.hex`
        -   `BUILD/NU_PFM_M2351_NPSA_S/ARM/cmse-lib.o`

    -   **M2354**:
        -   `BUILD/NU_M2354_NPSA_S/ARM/NuMaker-mbed-TZ-secure-example.hex`
        -   `BUILD/NU_M2354_NPSA_S/ARM/cmse-lib.o`

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

    -   **M2351**:
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
    -   **M2354**:
    ```json
    "target_overrides": {
        ......
        "NU_M2354_NPSA_NS": {
            ......
            "target.mbed_rom_start"             : "0x10080000",
            "target.mbed_rom_size"              : "0x80000",
            "target.mbed_ram_start"             : "0x30020000",
            "target.mbed_ram_size"              : "0x20000",
            ......
        }
    }
    ```

1.  Exclude pre-built secure image/gateway library

    There has been a pre-built secure code in mbed-os directory tree.
    To link with our own one as above, we must exclude the pre-built one.

    -   **M2351**:
    ```json
    "target_overrides": {
        ......
        "NU_PFM_M2351_NPSA_NS": {
            "target.extra_labels_remove"        : ["NU_PREBUILD_SECURE"],
            ......
        }
    }
    ```
    -   **M2354**:
    ```json
    "target_overrides": {
        ......
        "NU_M2354_NPSA_NS": {
            "target.extra_labels_remove"        : ["NU_PREBUILD_SECURE"],
            ......
        }
    }
    ```

1.  Add compiled secure code above into non-secure code build process

    Create one directory named below in the root of the example, and copy `NuMaker-mbed-TZ-secure-example.hex`/`cmse_lib.o` (just built above) there:

    -   **M2351**: `TARGET_NU_PFM_M2351_NPSA_NS`
    -   **M2354**: `TARGET_NU_M2354_NPSA_NS`

1.	Compile by running command:

    -   **M2351**:
    ```sh
    mbed compile -m NU_PFM_M2351_NPSA_NS -t ARM --app-config mbed_app_custom.json
    ```

    -   **M2354**:
    ```sh
    mbed compile -m NU_M2354_NPSA_NS -t ARM --app-config mbed_app_custom.json
    ```

1.  Flash compiled secure/non-secure code together

    Drag-n-drop below to flash compiled secure/non-secure code together:

    -   **M2351**:

    `BUILD/NU_PFM_M2351_NPSA_NS/ARM/NuMaker-mbed-TZ-nonsecure-example.hex` onto **NuMaker-PFM-M2351** board
    
    -   **M2354**:

    `BUILD/NU_M2354_NPSA_NS/ARM/NuMaker-mbed-TZ-nonsecure-example.hex` onto **NuMaker-M2354** board

## Execution

If everything goes well, you would see console log emitted by non-secure code like:

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

You would also see console log emitted by secure code in the front if serial is enabled in it.

**NOTE**: Configure your terminal program to **115200/8-N-1**.
