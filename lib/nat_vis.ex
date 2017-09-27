defmodule NatVis do
  @moduledoc """
  Documentation for ElixirNativeVisualization.
  """

  @on_load :load_nif

  def load_nif do
    :erlang.load_nif("_build/libNatVis", 0)
  end

  @doc """
  Open a window with SDL.
  """
  def open_window, do: raise "Not implemented!"

  def close_window(_window), do: raise "not implemented"

end

