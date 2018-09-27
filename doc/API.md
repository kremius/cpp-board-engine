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
    "boards": [
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
    "description": "Everything you wanted to be, but were afraid to admit",
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
A first element in the `posts` array is a thread id, others are thread preview posts. 

#### 4. Retrieve thread info
**Request**  
`GET` to `/<board>/<thread id>/info/` (e.g. to `/b/1/info/`)  

**Response**
```json
{
    "posts": [
        1,
        2,
        3,
        4,
        100,
        200,
        300,
        400
    ],
    "images": 2
}
```

#### 5. Retrieve posts
**Request**  
`GET` to `/<board>/posts?ids=<comma separated posts ids>` (e.g. to `/b/posts?ids=42,43`)  
List of the posts in the requested order.

**Response**
```json
{
    "posts": [
        {
            "id": 42,
            "text": "Hello world!",
            "image": "cat.png"
        },
        {
            "id": 43,
            "text": "Hello world 2!"
        }
    ]
}
```

#### 6. Retrieve posts in thread
**Request**  
`GET` to `/<board>/<thread id>/` (e.g. to `/b/42/`)  
List of post in a thread.

**Response**
```json
{
    "posts": [
        {
            "id": 42,
            "text": "Hello world!",
            "image": "cat.png"
        },
        {
            "id": 43,
            "text": "Hello world 2!"
        }
    ]
}
```

#### 7. Create thread
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
    "id": 123
}
```

#### 8. Post message
**Request**  
`POST` to `/<board>/<thread id>/`
```json
{
    "text": "Some stupid message!"
}
```
**TODO:** some way to post images

**Response**
```json
{
    "id": 124
}
```

**TODO:** response HTTP codes  
**TODO:** errors responses

