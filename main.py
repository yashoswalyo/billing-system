from fastapi import FastAPI, Response, status
from fastapi.middleware.cors import CORSMiddleware
import json
from utilities.database import db
from models import User, Product, UserIn, Message as m


def initMain():
    return print("Foo...bar...")


app = FastAPI(
    on_startup=initMain(),
)

origins = ["*"]

app.add_middleware(
    CORSMiddleware,
    allow_origins=origins,
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

@app.get("/")
def sendHello():
    return {"hello":"world"}

@app.get("/users")
def getAllUsers():
    cur = db.getAllUsers()
    u: list[User] = []
    for user in cur:
        user.pop("_id")
        user.pop("password")
        u.append(user)
    return u


@app.post("/user/register")
def registerUser(u: UserIn):
    db.addNewUser(u)
    return u


@app.post("/user/login", responses={400: {"model": m}})
def loginUser(u: UserIn, response: Response):
    res = db.getUserByName(u.username)
    if res is None:
        response.status_code = status.HTTP_400_BAD_REQUEST
        m.message = "User Not Found"
        m.status = "fail"
        return m
    res = UserIn.parse_obj(res)
    if u.password == res.password:
        res.isLoggedIn = True
        response.status_code = status.HTTP_201_CREATED
        return res
    else:
        response.status_code = status.HTTP_400_BAD_REQUEST
        m.message = "User Not Found"
        m.status = "fail"
        return m


@app.get("/user/{user_id}")
def getUser(user_id, resp: Response):
    u = db.getUserById(int(user_id))
    if u is not None:
        u = User.parse_obj(u)
        return u
    else:
        resp.status_code = status.HTTP_404_NOT_FOUND
        return {"status": "fail", "msg": "User not Found"}


@app.get("/user/{user_id}/products")
def getProductsForUser(user_id):
    res = db.getUserById(int(user_id))
    if res is None:
        m.status = "fail"
        m.message = "User ID invalid"
        return
    u = User.parse_obj(res)
    return u.products


@app.post("/product/new")
def addProduct(prod: Product):
    p = db.addNewProduct(prod)
    if p:
        m.status = "pass"
        m.message = "Product added"
        return 


@app.get("/products")
def getAllProducts() -> list[Product]:
    res = db.getAllProducts()
    l: list[Product] = []
    for i in res:
        i.pop("_id")
        l.append(Product.parse_obj(i))
    return l


@app.put("/order/{user_id}/products/{product_id}")
def addProductInCart(user_id: int, product_id: int, resp: Response):
    cur = db.getProductById(product_id)
    if cur is None:
        resp.status_code = status.HTTP_400_BAD_REQUEST
        return {"message": "Invalid product_id"}
    p = Product.parse_obj(cur)

    cur = db.getUserById(user_id)
    if cur is None:
        resp.status_code = status.HTTP_400_BAD_REQUEST
        return {"message": "Invalid user_id"}
    u = User.parse_obj(cur)

    cur = db.getOrder(u)
    cart = User.parse_obj(cur)

    for i in range(len(cart.products)):
        if p.product_id == cart.products[i].product_id:
            cart.products[i].product_qty += 1
            db.updateCart(cart)
            return cart

    cart.products.append(p)
    db.updateCart(cart)

    return cart


@app.get("/order/{user_id}/products")
def getUserOrderProducts(user_id):
    res = db.getUserById(int(user_id))
    if res is None:
        m.status = "fail"
        m.message = "User ID invalid"
        return
    u = User.parse_obj(res)
    res = db.getOrder(u)
    return User.parse_obj(res).products


@app.get("/order/{user_id}")
def getUserOrder(user_id):
    res = db.getUserById(int(user_id))
    if res is None:
        m.status = "fail"
        m.message = "User ID invalid"
        return
    u = User.parse_obj(res)
    res = db.getOrder(u)
    u = User.parse_obj(res)
    u.isLoggedIn = True
    return u


@app.delete("/order/{user_id}")
def deleteUserOrder(user_id: int):
    cur = db.getUserById(user_id)
    order = User.parse_obj(cur)

    order.products = []
    cur = db.updateCart(order)

    return order


@app.post("/user/logout/{user_id}")
def logoutUser(user_id: int):
    cur = db.getUserById(user_id)

    cart = User.parse_obj(cur)
    cart.isLoggedIn = False
    cart.isConnected = False
    db.updateCart(cart)

    user = UserIn.parse_obj(cur)
    user.isLoggedIn = False
    user.isConnected = False
    db.updateUser(user)
    return True


@app.post("/user/connect/{user_id}")
def connectUserById(user_id: int):
    cur = db.getUserById(user_id)

    cart = User.parse_obj(cur)
    cart.isConnected = True
    db.updateCart(cart)

    user = UserIn.parse_obj(cur)
    user.isConnected = True
    db.updateUser(user)
    return True;

@app.post("/user/disconnect/{user_id}")
def disconnectUserById(user_id: int):
    cur = db.getUserById(user_id)

    cart = User.parse_obj(cur)
    cart.isConnected = False
    db.updateCart(cart)

    user = UserIn.parse_obj(cur)
    user.isConnected = False
    db.updateUser(user)
    return True;


if __name__ == "__main__":
    import uvicorn
    import os

    uvicorn.run(app="main:app", host="0.0.0.0", port=int(os.getenv("PORT",6789)), reload=True)
