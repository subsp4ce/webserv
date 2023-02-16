# Webserv

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white) ![Nginx](https://img.shields.io/badge/nginx-%23009639.svg?style=for-the-badge&logo=nginx&logoColor=white)

Codam [42 Network] group project: write an HTTP server.

__Project description__

**asynchronous**, **non-blocking** HTTP server in C++ inspired by NGINX.

__Project requirements__

## Configuration file

The configuration file should contain at least one **server block** that is used to group configuration details for a specific domain. Multiple server blocks can be configured to host multiple domains on a single server. A server block can contain one or more **location blocks** to route requests for further configuration. Server and location blocks consist of **directives** and their parameters and end with a semicolon `;`. Comments are prefixed by a hash `#`.

| Server Block Directives | Parameters | Description
| ------ | ------ | ------ |
| `listen` | `host` or `host:port` or `port`| If not specified the default port is `80` and it will accept connections from any IP `0.0.0.0:80` |
| `server_name` | Either a single server name `example.com` or multiple server names `example` `example.org` `example.net` | If multiple server blocks have an exact match on the `listen` directive, the server looks for an exact match on the server name(s) of these blocks. If no exact match is found, the request will be handled by the first server block |
| `root` | `www/example/`  | Specifies the root directory that will be used to search for a file. Will be the default for any location block within that server block that doesn't specify a `root` or `alias` |
| `error_page` | `status_code` `custom_error_page` | Serve custom pages for certain HTTP response codes |
| `client_max_body_size` | `size_in_megabytes` | Limit the maximum size of a client's request body |

<br>

| Location Block Directives | Parameters | Description
| ------ | ------ | ------ |
| `allowed_methods` | `GET` and/or `POST` and/or `DELETE` | The allowed request methods for a specific route |
| `return` | either `301` `302` `303` `307` `308` followed by the `redirect_location` | Redirect a client to a new domain name |
| `root` | `www/example/` | Specifies the root directory that will be used to search for a file for a specific location. It overrides the root specified in the server block |
| `alias` | `/www/test` | Defines a replacement for the specified route. If `/example` is aliased to `/www/test` the url `/example/one/two/three` is `/www/test/one/two/three`. `alias` overrides both the root directive in the server and location block. |
| `autoindex` | Either `on` or `off` | Turn on or off directory listing (off by default) |
| `default_file` | Either a single file `index.html` or multiple files `index.html` `index.htm` | The server searches for the files in the specified order and returns the first one it finds. If `default_file` is not specified, `index.html` is set as the default file |
| `cgi` | `file_extention` `path_to_executable` | Executes cgi based an a certain file extension. Currently the execution of `.py` scripts are implemented |
| `file_upload` | `path_to_upload_location` | If not specified, files will be uploaded to the root directory |
