// <auto-generated>
//  automatically generated by the FlatBuffers compiler, do not modify
// </auto-generated>

namespace FlatPacket
{

using global::System;
using global::System.Collections.Generic;
using global::Google.FlatBuffers;

public struct item : IFlatbufferObject
{
  private Table __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public static void ValidateVersion() { FlatBufferConstants.FLATBUFFERS_24_3_25(); }
  public static item GetRootAsitem(ByteBuffer _bb) { return GetRootAsitem(_bb, new item()); }
  public static item GetRootAsitem(ByteBuffer _bb, item obj) { return (obj.__assign(_bb.GetInt(_bb.Position) + _bb.Position, _bb)); }
  public static bool Verifyitem(ByteBuffer _bb) {Google.FlatBuffers.Verifier verifier = new Google.FlatBuffers.Verifier(_bb); return verifier.VerifyBuffer("", false, itemVerify.Verify); }
  public void __init(int _i, ByteBuffer _bb) { __p = new Table(_i, _bb); }
  public item __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public int ItemId { get { int o = __p.__offset(4); return o != 0 ? __p.bb.GetInt(o + __p.bb_pos) : (int)0; } }
  public int ItemCount { get { int o = __p.__offset(6); return o != 0 ? __p.bb.GetInt(o + __p.bb_pos) : (int)0; } }

  public static Offset<FlatPacket.item> Createitem(FlatBufferBuilder builder,
      int item_id = 0,
      int item_count = 0) {
    builder.StartTable(2);
    item.AddItemCount(builder, item_count);
    item.AddItemId(builder, item_id);
    return item.Enditem(builder);
  }

  public static void Startitem(FlatBufferBuilder builder) { builder.StartTable(2); }
  public static void AddItemId(FlatBufferBuilder builder, int itemId) { builder.AddInt(0, itemId, 0); }
  public static void AddItemCount(FlatBufferBuilder builder, int itemCount) { builder.AddInt(1, itemCount, 0); }
  public static Offset<FlatPacket.item> Enditem(FlatBufferBuilder builder) {
    int o = builder.EndTable();
    return new Offset<FlatPacket.item>(o);
  }
  public static void FinishitemBuffer(FlatBufferBuilder builder, Offset<FlatPacket.item> offset) { builder.Finish(offset.Value); }
  public static void FinishSizePrefixeditemBuffer(FlatBufferBuilder builder, Offset<FlatPacket.item> offset) { builder.FinishSizePrefixed(offset.Value); }
}


static public class itemVerify
{
  static public bool Verify(Google.FlatBuffers.Verifier verifier, uint tablePos)
  {
    return verifier.VerifyTableStart(tablePos)
      && verifier.VerifyField(tablePos, 4 /*ItemId*/, 4 /*int*/, 4, false)
      && verifier.VerifyField(tablePos, 6 /*ItemCount*/, 4 /*int*/, 4, false)
      && verifier.VerifyTableEnd(tablePos);
  }
}

}
