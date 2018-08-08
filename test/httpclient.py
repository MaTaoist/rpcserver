#!/usr/bin/env python
# coding=utf-8
import http.client
import urllib
import json
url = '127.0.0.1:8080'

def cmdproess(cmd,data):
    try:
        con = http.client.HTTPConnection(url)
        con.request("POST", cmd,data)
        resu = con.getresponse()
        print('-'*60)
        print (resu.read().decode('utf-8'))
        print('-'*60)
        print(resu.status,resu.reason,resu.info())
        con.close()
    except Exception as e:
        print(e)

def parseUtxo(url):
    address = {'address':'16rsLLRw297mshn1Aa4jk9CHytnkf26ydV'}
    cmdlist = ['/getUtxo']
    datalist=[address]

    for i in range(0,len(cmdlist)):
        data = json.dumps(datalist[i])
        cmdproess(cmdlist[i],data)

def httpPost(url):

    postTx = {'txid':'17b726895a4897cfd400727b64dbb97746d6b87de74b7160b8d0c285713e50d8','tableid':'','hex':'020000000121b88c090b7a099bcd17da7579850fe30b1301f67473acfc79ba949a443dc12b000000006a473044022065b736431b5da318d5a1f24ff56e501bc64335fcec3dd6a1437819d031d0d7e80220605d40e5e060ad9b7fde50eaae28a9f8c4eebedbfd2a771e0f5063b0f7dc34510121026a946e991749ab10a95fc5ddb00e5089c09a73d1ad42b50287422d286bb78110feffffff02d4b85bee000000001976a9146e5f15ee7768ea3e871305c1469444654fc66fdb88ac00ca9a3b000000001976a9140ccb46c7f95a99dfc114d6d259597cf68c50a59c88ac06020000','content':'this is client match tx'}
    matchTx = [{'txid':'17b726895a4897cfd400727b64dbb97746d6b87de74b7160b8d0c285713e50d8','tableid':'2bc13d449a94ba79fcac7374f601130be30f857975da17cd9b097a0b098cb821'},{'txid':'6b510a025e8e367475ebd9c9cdb68c6b584d02a80e7dca7bb946e1ff530840a3','tableid':'69c6896dee3c329c4a3252e12af12125760e2559d36fc64c81e95baf0c5d43e2'}]

    getTableID = {'txid':'17b726895a4897cfd400727b64dbb97746d6b87de74b7160b8d0c285713e50d8'}

    postTxbet = {'txid':'17b726895a4897cfd400727b64dbb97746d6b87de74b7160b8d0c285713e50d8','tableid':'2bc13d449a94ba79fcac7374f601130be30f857975da17cd9b097a0b098cb821','hex':'020000000121b88c090b7a099bcd17da7579850fe30b1301f67473acfc79ba949a443dc12b000000006a473044022065b736431b5da318d5a1f24ff56e501bc64335fcec3dd6a1437819d031d0d7e80220605d40e5e060ad9b7fde50eaae28a9f8c4eebedbfd2a771e0f5063b0f7dc34510121026a946e991749ab10a95fc5ddb00e5089c09a73d1ad42b50287422d286bb78110feffffff02d4b85bee000000001976a9146e5f15ee7768ea3e871305c1469444654fc66fdb88ac00ca9a3b000000001976a9140ccb46c7f95a99dfc114d6d259597cf68c50a59c88ac06020000','content':'bet tx'}
    getTableTxs = {'tableid':'2bc13d449a94ba79fcac7374f601130be30f857975da17cd9b097a0b098cb821'}
    test = {'test':"hello world"}

    cmdlist = ['/postTx','/matchTx','/getTableID','/postTx','/getTableTxs','/test']
    datalist=[postTx,matchTx,getTableID,postTxbet,getTableTxs,test]

    for i in range(0,len(cmdlist)):
        data = json.dumps(datalist[i])
        cmdproess(cmdlist[i],data)

if __name__ == '__main__':
    parseUtxo(url)
