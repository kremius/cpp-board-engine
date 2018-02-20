# Board API
Base protocol: **HTTP**.
The API strives to be a REST-API.
Data format: **JSON** (both requests and responses).

### Base oprations

#### 1. Retrieve boards list
**Request**
`GET` to `/`

**Response**
```json
{
    "boards" : [
        "b",
        "vg",
        "soc"
    ]
}
```

#### 2. Retrieve board info
**Request**
`GET` to `/<board>/info/` (e.g. to `/b/info/`)

**Response**
```json
{
    "name": "b",
    "description": "Everything you wanted to be, but was afraid to admit",
    "pages": 10
}
```

#### 3. Retrieve threads list
**Request**
`GET` to `/<board>/<page number>/` (e.g. to `/b/1/`)
Just `/<board>/` will route to zero page (`/b/0/`)

**Response**
```json
{
    "threads": [
        {
            "posts": [
                1,
                2,
                4,
                1234
            ]
        },
        {
            "posts": [
                20,
                30,
                40,
                44
            ]
        }
    ]
}
```
#### 4. Retrieve post
**Request**
`GET` to `/<board>/<post number>/` (e.g. to `/b/42/`)
In case of a thread it will be a list of posts in the thread, otherwise the list will contain only one entry.
**TODO:** some way to request an arbitrary list of posts.

**Response**
```json
{
    "posts": [
        {
            "number": 42,
            "text": "Hello world!",
            "image": "cat.png"
        },
        {
            "number": 43,
            "text": "Hello world 2!"
        }
    ]
}
```

#### 5. Create thread
**Request**
`POST` to `/<board>/`
```json
{
    "text": "This is a new op post."
}
```
**TODO:** some way to post images

**Response**
```json
{
    "post": 123
}
```

#### 5. Post message
**Request**
`POST` to `/<board>/<thread number>/`
```json
{
    "text": "Some stupid message!"
}
```
**TODO:** some way to post images

**Response**
```json
{
    "post": 124
}
```

**TODO:** response HTTP codes
**TODO:** errors responses

