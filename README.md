# HBSRecover

QNAP Hybrid Backup Sync Recovery Tool

## In this repository

 * `hbsrecover`: A C++ implementation relying on Qt and Botan
 * `pyhbsrecover`: A Python implementation relying on PyCrypto 

## Credits

Inspired from [Mikiya83](https://github.com/Mikiya83)'s [hbs_decipher](https://github.com/Mikiya83/hbs_decipher). 

[Mikiya83](https://github.com/Mikiya83)'s project does not share the limitations below. It implements everything.

I just did not want to depend on Java... my bad.

## Limitations

| Hybrid Backup Sync Format | Supported by                    | Comment                                |
|:-------------------------:|:-------------------------------:|:---------------------------------------|
|           HBSv1           | None                            | Too old to be worth coding it          |
|    HBSv2 w/ compression   | None                            | Maybe later                            |
|   HBSv2 w/o compression   | `pyhbsrecover` and `hbsrecover` | Done                                   |
|    HBSv3 w/ compression   | None                            | Maybe later                            |
|   HBSv3 w/o compression   | `pyhbsrecover`                  | I'll do the C++ implementation later   |
