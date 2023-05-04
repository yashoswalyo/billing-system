from pymongo import MongoClient
from bson import json_util
from models import User, UserIn,Product
import json
import os

MONGODB_API = "FILL_YOUR_MONGODB_API"


class Database:
    def __init__(self) -> None:
        self.client = MongoClient(MONGODB_API)
        self.shop = self.client.ShopData
        self.users = self.shop.users
        self.products = self.shop.products
        self.orders = self.shop.orders
        


    def addNewUser(self, user: UserIn):
        print(user.json())
        data = json.loads(user.json())
        print(data)
        self.users.insert_one(data)
        return

    def getAllUsers(self):
        cur = self.users.find({})
        return cur

    def getUserByName(self,username:str):
        cur = self.users.find_one({"username":username})
        if cur is None:
            return None
        cur.pop("_id")
        return cur

    def getUserById(self,uid:int):
        cur = self.users.find_one({"uid":uid})
        if cur is None:
            return None
        cur.pop("_id")
        return cur

    def addNewProduct(self,product:Product):
        cur = self.products.insert_one(product.dict())
        return True if cur.acknowledged else False

    def getAllProducts(self):
        cur = self.products.find({})
        return cur
    
    def getProductById(self, product_id):
        cur = self.products.find_one({"product_id":product_id})
        if cur is None:
            return None
        cur.pop("_id")
        return cur

    def updateCart(self,user:User):
        cur = self.orders.find_one_and_replace({"uid":user.uid}, user.dict())
        return cur
    
    def getOrder(self, user:User):
        cur = self.orders.find_one({"uid":user.uid})
        if cur is None:
            print("Order not found, Creating new order")
            self.orders.insert_one(user.dict())
            return self.getOrder(user) 
        else:
            cur.pop("_id")
            return cur
    
    def updateUser(self, user:UserIn):
        cur = self.users.find_one_and_replace({"uid":user.uid},user.dict())
        return cur
        



db = Database()
