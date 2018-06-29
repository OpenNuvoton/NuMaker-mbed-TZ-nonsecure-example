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

## Partition flash/SRAM/peripherals

TODO

## Compile TrustZone secure code

Follow the steps below to compile TrustZone secure code.

1. Clone NuMaker-mbed-TZ-secure-example

    `mbed import https://github.com/OpenNuvoton/NuMaker-mbed-TZ-secure-example`

    `cd NuMaker-mbed-TZ-secure-example`

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
    To jump to non-secure address `0x10024000`, `mbed_app.json` would have:
   
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
                <b>"tz-start-ns": "0x10024000",</b>
                "target.core": "Cortex-M23",
                "target.inherits": ["NUMAKER_PFM_M2351"],
                "target.device_has_remove": ["TRNG", "SERIAL", "SERIAL_ASYNCH", "SERIAL_FC", "STDIO_MESSAGES"]
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

1.  Copy secure gateway library

    Create TARGET_NUMAKER_PFM_M2351_NS directory and copy `cmse_lib.o` (just built above) here.
    
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
