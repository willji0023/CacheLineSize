# CacheLineSize
By the Ajorpaz/Tullsen Group

[Notes](https://docs.google.com/document/d/1To7tvY3saYzxBsnUqYrxLg5Z04TeZs-51bB1LkjfwW8/edit)

## Viewing CSV Files
View the .csv file by uploading it to
https://www.csvplot.com/

## HW Prefetcher

To enable/disable hardware prefecture, first run `cd msr-tools`.

Make the executables using `make`.

### Disabling HW Prefetcher
`./wrmsr -p 0 0x1a0 0x60628e2289 `

`./rdmsr 0x1a0`

### Enabling HW Prefetcher
`./wrmsr -p 0 0x1a0 0x60628e2089`

`./rdmsr 0x1a0`
