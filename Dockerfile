FROM python:3.9
WORKDIR /code
COPY ./requirements.txt .
RUN pip3 install --no-cache-dir -r requirements.txt
COPY . /code/
CMD [ "python3","main.py"]
