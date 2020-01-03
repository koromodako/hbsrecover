# HBSRecover

QNAP Hybrid Backup Sync Recovery Tool

## In this repository

 * A native Qt and Botan powered application
 * A Python script relying on PyCrypto and native python modules

## Credits

Inspired from [Mikiya83](https://github.com/Mikiya83)'s [hbs_decipher](https://github.com/Mikiya83/hbs_decipher). 

[Mikiya83](https://github.com/Mikiya83)'s project does not share the limitations below. It implements everything.

I just did not want to depend on Java... my bad.

## Limitations

| Hybrid Backup Sync Format | Supported ? | Comment                                              |
|:-------------------------:|:-----------:|:-----------------------------------------------------|
|           HBSv1           |     No      | Too old to be worth coding it                        |
|    HBSv2 w/ compression   |     No      | Compressing before encrypting does not seem smart ;) |
|   HBSv2 w/o compression   |     Yes     | This is the only one I need for the moment           |
|   OpenSSL w/ compression  |     No      | Compressing before encrypting does not seem smart ;) |
|  OpenSSL w/o compression  |     No      | I lack free time, when I need it maybe               |
