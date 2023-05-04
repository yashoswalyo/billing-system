from pydantic import BaseModel
import random

class Product(BaseModel):
  product_id:int
  product_name:str
  product_image_url:str
  product_qty:int
  product_price:int

class User(BaseModel):
  uid:int = 0
  username:str
  isLoggedIn:bool = False
  isConnected:bool = False
  products:list[Product] = []
  
class UserIn(User):
  password:str 


class Message(BaseModel):
  status:str
  message:str

